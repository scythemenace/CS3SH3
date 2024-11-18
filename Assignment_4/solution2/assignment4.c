#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#define BUFFER_SIZE 10
#define OFFSET_BITS 8
#define PAGE_SIZE 256          // 2^8
#define OFFSET_MASK 0xFFFFFFFF // Offset is the remainder and in this case the lower 12 bits. Each hex 'F' represent 8 bits
#define PAGES 256              // 2^8
#define FRAME_COUNT 128

int main()
{
  int page_table[PAGES];
  for (int i = 0; i < PAGES; i++)
  {
    page_table[i] = -1;
  }


  // Writing .bin file to memory
  signed char *mmapfptr; // To store the starting address of the memory mapped file
  int mmapfile_fd = open("BACKING_STORE.bin", O_RDONLY);
  if (mmapfile_fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
  }

  mmapfptr = mmap(NULL, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
  // END

  // Creating a phyiscal memory using circular array
  signed char physical_memory[FRAME_COUNT][PAGE_SIZE];
  int current_frame_index = 0;

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
  // END

  FILE *fptr = fopen("addresses.txt", "r");
  if (fptr == NULL)
  {
    fprintf(stderr, "Error opening file.\n");
    return -1;
  }

  char buff[BUFFER_SIZE];
  while (fgets(buff, BUFFER_SIZE, fptr) != NULL)
  {

    unsigned int logical_address = (unsigned int)atoi(buff);
    printf("Logical address being translated %u\n", logical_address);

    unsigned int page_number = logical_address >> OFFSET_BITS; // essentially logical_address / offset_bits
    unsigned offset = logical_address & OFFSET_MASK;           // Getting the remainder i.e. the lower 12 bits

    if (page_number >= PAGES)
    {
      fprintf(stderr, "Invalid page number: %u\n", page_number);
      continue;
    }

    /* Opposite of getting the page number from the address, now we are getting the address from the page number by multiplying the frame
     * number with the page size (the reason we use offset bits here is in << by multiply by 2^(bits) and in this case 2^(12 - offset bits) = page size)
     * and adding the offset for that particular page in the logical address that we calculated*/

    if (page_table[page_number] != -1) {
      unsigned int frame_number = page_table[page_number];
    } else {
      replace_page(page_number);
      unsigned int frame_number = page_table[page_number];
    }

    unsigned int physical_address = (frame_number << OFFSET_BITS) | offset;
    printf("The corresponding physical address is: %u\n", physical_address);
    signed char value = physical_memory[frame_number][offset];
    printf("The signed byte value at %u is %c\n", physical_address, value);
  }

  fclose(fptr);

  return 0;
}
