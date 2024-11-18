# Documentation for the solution of Assignment 4

## Sections of each problem statement

This documentation will divide each task into different problem statements and attempt to solve each problem in different iterations

### Initial setup

The lab 4 files are the primary source of help for the setup required to solve this assignment.

We will copy the initial boilerplate from there

```c
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
     * and adding the e offset for that particular page in the logical address that we calculated*/
    unsigned int frame_number = page_table[page_number];
    unsigned int physical_address = (frame_number << OFFSET_BITS) | offset;

    printf("Virtual addr is %u: Page# = %u & Offset = %u. Physical addr = %u.\n", logical_address, page_number, offset, physical_address);
  }

  fclose(fptr);

  return 0;
}
```

## Checking if it can read addresses.txt correctly

I will proceed to check if it reads all the addresses in the `addresses.txt` file.

```c
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

  FILE *fptr = fopen("addresses.txt", "r");
  if (fptr == NULL) {
    fprintf(stderr, "Error opening file.\n");
    return -1;
  }

  char buff[BUFFER_SIZE];
  while (fgets(buff, BUFFER_SIZE, fptr) != NULL) {

    printf("%s", buff);
  }

  fclose(fptr);

  return 0;
}
```

It seems to read the file without any issues.

## Defining values according to assignment requirements

- **Logical address size**: $2^{16}$ or 65536 bytes.
- **Physical address size**: $2^{15}$ bytes.
- **Bits needed to represent addresses in the logical address space**: Logical address space is $2^{16}$ bytes that means 16 bits will be required to indicate all the different addresses or you could say the maximum address space spans to $2^{16}$ - 1 = 65535 which is 1111111111111111 (16 digits) therefore, we need 16 digits to indicate all addresses.

- **Bits needed to represent addresses in the physical address space**: Similarly, we need 15 bits to indicate all the different addresses in the physical space
- **Page size**: 256 bytes or $2^{8}$ bytes.
- **Number of pages**: As per the slides, we can deduce the total number of pages in the logical space would be $\cfrac{\text{size of the logical space}}{\text{size of one page}}$ = $\cfrac{2^{16}}{2^{8}}$
