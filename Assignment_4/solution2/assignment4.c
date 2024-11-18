#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define LOGICAL_ADDRESS_SIZE 65536
#define OFFSET_BITS 8
#define PAGE_SIZE 256    // 2^8
#define OFFSET_MASK 0xFF // Masks lower 8 bits
#define PAGES 256        // 2^8
#define FRAME_COUNT 128
#define TLB_SIZE 16 // Typical small size for TLB

typedef struct
{
  int page_number;
  int frame_number;
} TLBentry;

// Global variables
int page_table[PAGES];
signed char physical_memory[FRAME_COUNT][PAGE_SIZE];
int current_frame_index = 0;
signed char *mmapfptr;

// TLB variables
TLBentry TLB[TLB_SIZE];
int TLBindex = 0; // Points to the next position to insert in TLB (FIFO)
int TLBcount = 0; // Number of entries currently in TLB

// Function declarations
signed char read_frames(int frame_index, int offset);
void write_frame(int page_number, int frame_index);
void replace_page(int page_number);
int search_TLB(int page_number);
void TLB_Add(int page_number, int frame_number);
void TLB_Update(int page_number, int frame_number);

int main()
{
  // Initialize page_table and TLB
  for (int i = 0; i < PAGES; i++)
  {
    page_table[i] = -1;
  }
  for (int i = 0; i < TLB_SIZE; i++)
  {
    TLB[i].page_number = -1;
    TLB[i].frame_number = -1;
  }

  // Open the .bin file
  int mmapfile_fd = open("BACKING_STORE.bin", O_RDONLY);
  if (mmapfile_fd == -1)
  {
    perror("Error opening BACKING_STORE.bin");
    exit(EXIT_FAILURE);
  }

  // Memory map the .bin file
  mmapfptr = mmap(NULL, LOGICAL_ADDRESS_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
  if (mmapfptr == MAP_FAILED)
  {
    perror("Error mapping BACKING_STORE.bin");
    close(mmapfile_fd);
    exit(EXIT_FAILURE);
  }

  // Open addresses.txt
  FILE *fptr = fopen("addresses.txt", "r");
  if (fptr == NULL)
  {
    fprintf(stderr, "Error opening addresses.txt.\n");
    munmap(mmapfptr, LOGICAL_ADDRESS_SIZE);
    close(mmapfile_fd);
    return -1;
  }

  char buff[BUFFER_SIZE];
  int total_addresses = 0;
  int TLB_hits = 0;
  int page_faults = 0;

  while (fgets(buff, BUFFER_SIZE, fptr) != NULL)
  {
    unsigned int logical_address = (unsigned int)atoi(buff);
    // printf("Logical address being translated: %u\n", logical_address);

    unsigned int page_number = logical_address >> OFFSET_BITS; // Get page number
    unsigned int offset = logical_address & OFFSET_MASK;       // Get offset

    if (page_number >= PAGES)
    {
      fprintf(stderr, "Invalid page number: %u\n", page_number);
      continue;
    }

    total_addresses++;

    int frame_number = search_TLB(page_number);
    if (frame_number != -1)
    {
      // TLB hit
      TLB_hits++;
      // No need to do anything else
    }
    else
    {
      // TLB miss
      if (page_table[page_number] != -1)
      {
        // Page is in memory
        frame_number = page_table[page_number];
      }
      else
      {
        // Page fault, load page into memory
        replace_page(page_number);
        frame_number = page_table[page_number];
        page_faults++;
      }
      // Add the new page and frame number to the TLB
      TLB_Add(page_number, frame_number);
    }

    unsigned int physical_address = (frame_number << OFFSET_BITS) | offset;
    printf("The corresponding physical address is: %u\n", physical_address);

    signed char value = read_frames(frame_number, offset);
    printf("The signed byte value at %u is %d\n", physical_address, value);
  }

  // Print statistics
  printf("Page Faults = %d\n", page_faults);
  printf("TLB Hits = %d\n", TLB_hits);

  // Cleanup
  fclose(fptr);
  munmap(mmapfptr, LOGICAL_ADDRESS_SIZE);
  close(mmapfile_fd);

  return 0;
}

// Reads a byte from physical memory
signed char read_frames(int frame_index, int offset)
{
  return physical_memory[frame_index][offset];
}

// Writes a page from backing store to a frame in physical memory
void write_frame(int page_number, int frame_index)
{
  memcpy(physical_memory[frame_index], mmapfptr + page_number * PAGE_SIZE, PAGE_SIZE);
}

// Replaces a page in physical memory using FIFO policy
void replace_page(int page_number)
{
  write_frame(page_number, current_frame_index);
  page_table[page_number] = current_frame_index;

  // Update TLB if necessary
  TLB_Update(page_number, current_frame_index);

  current_frame_index = (current_frame_index + 1) % FRAME_COUNT;
}

// Searches the TLB for a page number, returns frame number or -1 if not found
int search_TLB(int page_number)
{
  for (int i = 0; i < TLB_SIZE; i++)
  {
    if (TLB[i].page_number == page_number)
    {
      return TLB[i].frame_number;
    }
  }
  return -1; // Not found
}

// Adds a page and frame number to the TLB using FIFO replacement policy
void TLB_Add(int page_number, int frame_number)
{
  TLB[TLBindex].page_number = page_number;
  TLB[TLBindex].frame_number = frame_number;
  TLBindex = (TLBindex + 1) % TLB_SIZE;

  if (TLBcount < TLB_SIZE)
    TLBcount++;
}

// Updates the TLB when a page is replaced in physical memory
void TLB_Update(int page_number, int frame_number)
{
  for (int i = 0; i < TLB_SIZE; i++)
  {
    if (TLB[i].page_number == page_number)
    {
      // Update the frame number at the same index
      TLB[i].frame_number = frame_number;
      return;
    }
  }
  // If not found, do nothing
}
