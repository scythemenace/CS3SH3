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
#define OFFSET_MASK 0xFF // Corrected to mask lower 8 bits
#define PAGES 256        // 2^8
#define FRAME_COUNT 128
#define TLB_SIZE 16

// Creating a custom type to store page_number and frame_number as a pair
typedef struct {
  int page_number;
  int frame_number;
} TLBentry;

TLBentry tlb[TLB_SIZE];

// Initializing a pointer to keep track of next available index
int tlb_next_index = 0;

int search_tlb(int page_number) {
  // Loop through each entry in the tlb to find if we have that page_number
  for (int i = 0; i < TLB_SIZE; i++) {
    if (tlb[i].page_number == page_number) {
      return tlb[i].frame_number;
    }
  }

  return -1; // not found
}

void add_tlb(int page_number, int frame_number) {
  // updating our values based on the pointer that we explicitly created
  tlb[tlb_next_index].page_number = page_number;
  tlb[tlb_next_index].frame_number = frame_number;

  // Logic to update the tlb_next_index in the circular array fashion
  tlb_next_index = (tlb_next_index + 1) % TLB_SIZE;
}

void update_tlb(int old_page_number, int new_page_number, int new_frame_number) {
  for (int i = 0; i < TLB_SIZE; i++) {
    if (tlb[i].page_number == old_page_number) {
      tlb[i].page_number = new_page_number;
      tlb[i].frame_number = new_frame_number;
      return;
    }
  }
}

int page_table[PAGES];
signed char physical_memory[FRAME_COUNT][PAGE_SIZE];
int current_frame_index = 0;
signed char *mmapfptr;

signed char read_frames(int frame_index, int offset)
{
  return physical_memory[frame_index][offset];
}

void write_frame(int page_number, int frame_index)
{
  memcpy(physical_memory[frame_index], mmapfptr + page_number * PAGE_SIZE, PAGE_SIZE);
}

void replace_page(int page_number)
{
  write_frame(page_number, current_frame_index);
  page_table[page_number] = current_frame_index;
  current_frame_index = (current_frame_index + 1) % FRAME_COUNT;
}

int main()
{
  // Initialize page_table
  for (int i = 0; i < PAGES; i++)
  {
    page_table[i] = -1;
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
  while (fgets(buff, BUFFER_SIZE, fptr) != NULL)
  {
    unsigned int logical_address = (unsigned int)atoi(buff);
    printf("Logical address being translated: %u\n", logical_address);

    unsigned int page_number = logical_address >> OFFSET_BITS; // Get page number
    unsigned int offset = logical_address & OFFSET_MASK;       // Get offset

    if (page_number >= PAGES)
    {
      fprintf(stderr, "Invalid page number: %u\n", page_number);
      continue;
    }

    unsigned int frame_number;

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
    }

    unsigned int physical_address = (frame_number << OFFSET_BITS) | offset;
    printf("The corresponding physical address is: %u\n", physical_address);

    signed char value = read_frames(frame_number, offset);
    printf("The signed byte value at %u is %d\n", physical_address, value);
  }

  // Cleanup
  fclose(fptr);
  munmap(mmapfptr, LOGICAL_ADDRESS_SIZE);
  close(mmapfile_fd);

  return 0;
}
