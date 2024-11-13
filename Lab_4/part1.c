#include <stdio.h>
#include <stdlib.h>

//As stated, the logical addresses are no more than size 10
#define BUFFER_SIZE 10
#define OFFSET_BITS 12
#define PAGE_SIZE 4096 // 2^12
#define OFFSET_MASK 0xFFF // Offset is the remainder and in this case the lower 12 bits. Each hex 'F' represent 4 bits and 3 F's (FFF) represent 12 bits
#define PAGES 8

int main() {
  int page_table[PAGES] = {6, 4, 3, 7, 0, 1, 2, 5};

  FILE *fptr = fopen("labaddr.txt", "r");
  if (fptr == NULL) {
    fprintf(stderr, "Error opening file.\n");
    return -1;
  }

  char buff[BUFFER_SIZE];
  while (fgets(buff, BUFFER_SIZE, fptr) != NULL) {
    unsigned int logical_address = (unsigned int)atoi(buff);
    unsigned int page_number = logical_address >> OFFSET_BITS; // essentially logical_address / offset_bits
    unsigned offset = logical_address & OFFSET_MASK; // Getting the remainder i.e. the lower 12 bits
    
    if (page_number >= PAGES) {
      fprintf(stderr, "Invalid page number: %u\n", page_number);
      continue;
    }

    /* Opposite of getting the page number from the address, now we are getting the address from the page number by multiplying the frame 
     * number with the page size (the reason we use offset bits here is in << by multiply by 2^(bits) and in this case 2^(12 - offset bits) = page size)
     * and adding the offset for that particular page in the logical address that we calculated*/
    unsigned int frame_number = page_table[page_number];
    unsigned int physical_address = (frame_number << OFFSET_BITS) | offset;

    printf("Virtual addr is %u: Page# = %u & Offset = %u. Physical addr = %u.\n", logical_address, page_number, offset, physical_address);
  }

  fclose(fptr);
  
  return 0;
}
