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
#define PAGES 256        // Offset is the remainder and in this case the lower 8 bits. Each hex 'F' represent 4 bits, therefore two F's (FF) represent 8 bits
#define FRAME_COUNT 128
#define TLB_SIZE 16 // As given in the TLB

// Creating a custom type for storing page_number and frame_number
typedef struct
{
  int page_number;
  int frame_number;
} TLBentry;

int page_table[PAGES];
/*Creating a 2-D array to simulate a physical memory, normally a physical memory has
 * pages which have contiguous locations as its part, which is true here as well but
 * instead of initializing an array of size 2^15, constructing it in the form of a 2-D
 * gives us simplicity in working with the array*/
signed char physical_memory[FRAME_COUNT][PAGE_SIZE];

int current_frame_index = 0; // Using it as a pointer to mark the next available index in our physical memory since it's implemented as a circular array
signed char *mmapfptr;

// TLB variables
TLBentry TLB[TLB_SIZE];
int TLBindex = 0; // Points to the next position to insert in TLB (similar to the physical memory logic)
int TLBcount = 0; // Number of entries currently in TLB

// Function prototypes
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

  // Memory mapping the BACKING_STORE.bin file
  int mmapfile_fd = open("BACKING_STORE.bin", O_RDONLY);
  // I have added a safety check in case I have spelling mistakes in my filenames
  if (mmapfile_fd == -1)
  {
    perror("Error opening BACKING_STORE.bin");
    exit(EXIT_FAILURE);
  }

  mmapfptr = mmap(NULL, LOGICAL_ADDRESS_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
  // END

  // Open addresses.txt
  FILE *fptr = fopen("addresses.txt", "r");
  // I have added a safety check in case I have spelling mistakes in my filenames
  if (fptr == NULL)
  {
    fprintf(stderr, "Error opening addresses.txt.\n");
    munmap(mmapfptr, LOGICAL_ADDRESS_SIZE);
    close(mmapfile_fd);
    return -1;
  }

  char buff[BUFFER_SIZE]; // To store the opened file's values which will later be casted as unsigned ints

  int TLB_hits = 0;    // Variable to keep track of the number of TLB hits that have occurred
  int page_faults = 0; // Variable to keep track of the number of page faults that have occurred

  // Loops through the whole addresses.txt file which simulates behaviour of someone requesting memory addresses
  while (fgets(buff, BUFFER_SIZE, fptr) != NULL)
  {
    unsigned int logical_address = (unsigned int)atoi(buff);
    printf("Logical address being translated: %u\n", logical_address);

    unsigned int page_number = logical_address >> OFFSET_BITS; // Essentially logical_address / offset_bits in binary
    unsigned int offset = logical_address & OFFSET_MASK;       // Getting the remainder i.e. the lower 8 bits

    // Just a safety check to ensure we don't have any errors with the pages
    if (page_number >= PAGES)
    {
      fprintf(stderr, "Invalid page number: %u\n", page_number);
      continue;
    }

    int frame_number = search_TLB(page_number); // We first look in the TLB
    if (frame_number != -1)
    {
      // Updating the TLB hit so that we can print the total tlb hits in the end
      TLB_hits++;
    }
    else
    {
      // TLB miss
      if (page_table[page_number] != -1) // If it exists in physical memory
      {
        frame_number = page_table[page_number]; // We get the frame number from the page_table
      }
      else
      {
        // A Page fault has occured due to the neither the tlb nor the physical memory having the frame number
        replace_page(page_number);
        frame_number = page_table[page_number]; // Updating the frame number in the page_table
        page_faults++;
      }
      // Add the new page and frame number to the TLB
      TLB_Add(page_number, frame_number);
    }

    /* Opposite of getting the page number from the address
     * now we are getting the address from the page number by multiplying the frame number with the page size and adding the
     * offset for that particular page in the logical address that we calculated */
    unsigned int physical_address = (frame_number << OFFSET_BITS) | offset;
    printf("The corresponding physical address is: %u\n", physical_address);

    signed char value = read_frames(frame_number, offset);
    printf("The signed byte value at %u is %d\n", physical_address, value);
  }

  // Print additional statistics as per the assignment requirements
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

// Writes a page from the memory mapping of BACKING_STORE to a frame in physical memory
void write_frame(int page_number, int frame_index)
{
  // The key here is how memcpy() works in our write_frame() function:
  // 1. The first argument of memcpy() is the destination, which is the physical_memory's
  //    nested array corresponding to the target frame (physical_memory[frame_index]).
  // 2. The second argument is the source location in the memory-mapped file.
  //    From lab assignments, we learned that to copy specific data from a memory-mapped file:
  //        memcpy(&intArray[i], mmapfptr + 4 * i, 4);
  //    - This offsets `mmapfptr` by the current position (4 * i) to locate the next integer
  //      since each integer is 4 bytes.
  // 3. In our case, each signed char takes 1 byte, so for copying an entire page:
  //    - Use `mmapfptr + page_number * PAGE_SIZE` as the source (offset to the page's location).
  //    - The second argument (`page_number * PAGE_SIZE`) calculates the starting position of
  //      the page in the memory-mapped file.
  // 4. The third argument is 256 (PAGE_SIZE), since:
  //    - Each page consists of 256 bytes (signed chars).
  //    - We copy the entire page to the destination frame in physical_memory.
  memcpy(physical_memory[frame_index], mmapfptr + page_number * PAGE_SIZE, PAGE_SIZE);
}

// Replaces a page in physical memory using FIFO policy
void replace_page(int page_number)
{
  write_frame(page_number, current_frame_index);
  page_table[page_number] = current_frame_index; // Update the page_table as well

  // Update TLB if necessary (if the old page exists and hasn't been replaced in the physical memory, nothing will happen, otherwise new frame will take the position of the existing page number)
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

// Adds a page and frame number to the TLB
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
      // We update the frame number at the same index
      TLB[i].frame_number = frame_number;
      return;
    }
  }
  // If not found, we don't do anything
}
