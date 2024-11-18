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
- **Number of pages**: As per the slides, we can deduce the total number of pages in the logical space would be $\cfrac{\text{size of the logical space}}{\text{size of one page}}$ = $\cfrac{2^{16}}{2^{8}}$ = $2^{16 - 8}$ = $2^8$
- **Number of bits required to represent the page number**: 8 bits due to the fact that there are $2^8$ different pages.
- **Number of bits required to represent the offset**: 8 bits because logically think about this, a single page is of size 256 bytes or $2^8$ bytes. That means we need 8 bits to represent each different location in one page, therefore we need 8 bits for the offset since that's literally what the offset does. Alternatively, you can think of this like this: Each address has 16 bits, if 8 were used to denote the page number then 16 - 8 = 8 bits will be used to denote the offset.
- **Number of frames**: Since frame size = page size, we can calculate number of frames by dividing total physical memory size by the frame size i.e. total number of frames = $\cfrac{\text{size of physical memory}}{\text{size of page}}$ = $\cfrac{2^{15}}{2^8}$ = $2^{15 - 8}$ = $2^7$
- **Number of bits needed to represent each frame number**: 7 bits due to reasons already explained above for different examples.
- **Maximum number of entries in the TLB**: 16

## Basic breakdown of MMU simulator excluding the TLB

- We are required to translate logical addresses to physical address.
- First we test our program by changing the values in our boilerplate code with the new values based on our deduction

```c
#define BUFFER_SIZE 10
#define OFFSET_BITS 8
#define PAGE_SIZE 256 // 2^8
#define OFFSET_MASK 0xFFFFFFFF // Offset is the remainder and in this case the lower 12 bits. Each hex 'F' represent 8 bits
#define PAGES 256 // 2^8
```

- Then we proceed to initalize a page_table of size 256. We do so by iterating through the array and giving each index a value of -1.

```c
int page_table[PAGES];
  for (int i = 0; i < PAGES; i++) {
    page_table[i] = -1;
  }
```

We need a circular array of type `signed char` and size 128 since we have 128 different pages. This can be a 2-D array where each index is of size 256 representing the page size. This is perfect due to the fact that `signed char` takes up 1 byte and our mmap() function also usually uses the `signed char` type to map values.

1. We intialize the 2-D array:

```c
signed char physical_memory[FRAME_COUNT][FRAME_SIZE];
```

2. We use a pointer to keep track of which page to replace

```c
int current_frame_index = 0;
```

When a page needs to be replaced:

- Overwrite the current frame at the current_frame_index
- Increment the pointer and in case of overflow just use modulo arithmetic to wrap it around

```c
current_frame_index = (current_frame_index + 1) % FRAME_COUNT
```

3. For reading an address we access, the required address if given two inputs, 1. Frame Index and 2. Offset

```c
signed char read_frames(int frame_index, int offset) {
    return physical_memory[frame_index][offset];
  }
```

4. Handling page replacements by using the current_frame_index to identify which frame to replace next

```c
void replace_page(int page_number, signed char *new_page_data) {
    write_frame(current_frame_index, new_page_data);
    page_table[page_number] = current_frame_index;
    current_frame_index = (current_frame_index + 1) % FRAME_COUNT;
  }
```

To do this, we use another helper function called `write_frame` written below.

5. We use write frame for replacing the contents of the frame in case of a page fault

```c
void write_frame(int frame_index, signed char *data) {
    for (int i = 0; i < FRAME_SIZE; i++) {
      physical_memory[frame_index][i] = data[i]
    }
  }
```
