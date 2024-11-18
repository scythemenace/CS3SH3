#include <stdio.h>
#include <stdlib.h>

//As stated, the logical addresses are no more than size 10
#define BUFFER_SIZE 10
#define OFFSET_BITS 8
#define PAGE_SIZE 256 // 2^8
#define OFFSET_MASK 0xFFFFFFFF // Offset is the remainder and in this case the lower 12 bits. Each hex 'F' represent 8 bits 
#define PAGES 256 // 2^8
#define FRAME_COUNT 128
#define FRAME_SIZE 256

int main() {
  int page_table[PAGES];
  for (int i = 0; i < PAGES; i++) {
    page_table[i] = -1;
  }

  // Creating a phyiscal memory using circular array
  signed char physical_memory[FRAME_COUNT][FRAME_SIZE];
  int current_frame_index = 0;

  signed char read_frames(int frame_index, int offset) {
    return physical_memory[frame_index][offset];
  }

  void write_frame(int frame_index, signed char *data) {
    for (int i = 0; i < FRAME_SIZE; i++) {
      physical_memory[frame_index][i] = data[i]
    }
  }

  void replace_page(int page_number, signed char *new_page_data) {
    write_frame(current_frame_index, new_page_data);
    page_table[page_number] = current_frame_index;
    current_frame_index = (current_frame_index + 1) % FRAME_COUNT;
  }
  // END

  FILE *fptr = fopen("addresses.txt", "r");
  if (fptr == NULL) {
    fprintf(stderr, "Error opening file.\n");
    return -1;
  }

  char buff[BUFFER_SIZE];
  while (fgets(buff, BUFFER_SIZE, fptr) != NULL) {
     
  unsigned int logical_address = (unsigned int)atoi(buff);
  printf("%u\n", logical_address);

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
