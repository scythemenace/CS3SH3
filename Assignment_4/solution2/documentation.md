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

4. Handling page replacements by using the current_frame_index to identify which frame to replace next. Additionally to invalidate old pages, we initialize an inverse mapping of frame to pages

```c
int frame_page[FRAME_COUNT]; // For inverse mapping to update page_table
```

```c
// Replaces a page in physical memory using FIFO policy
void replace_page(int page_number)
{
  int old_page = frame_page[current_frame_index];
  if (old_page != -1)
  {
    // Invalidate the old page's entry in the page table
    page_table[old_page] = -1;
  }

  frame_page[current_frame_index] = page_number;

  write_frame(page_number, current_frame_index);
  page_table[page_number] = current_frame_index; // Update the page_table as well

  // Update TLB if necessary (if the old page exists and hasn't been replaced in the physical memory, nothing will happen, otherwise new frame will take the position of the existing page number)
  update_TLB(page_number, current_frame_index);

  current_frame_index = (current_frame_index + 1) % FRAME_COUNT;
}
```

We need to invalidate the old page entry at the beginning. Failure to do so resulted in less page faults being counted, since their addresses were being updated but the page_table wasn't correctly being updated. Therefore, whenever a page that doesn't existed came up again, the if condition would consider it to be there still causing less page faults to be detected.

To do this, we use another helper function called `write_frame` written below.

5. We use write frame for replacing the contents of the frame in case of a page fault

```c
void write_frame(int page_number, int frame_index)
{
  memcpy(physical_memory[frame_index], mmapfptr + page_number * PAGE_SIZE, PAGE_SIZE);
}
```

At the beginning section of the code, we should probably first copy the .bin file to memory since in the initial phases everything will be a page fault since the page_table is intialized to -1. Therefore, it is crucial that we have the bin file in the memory

We first write the .bin file to memory

```c
signed char *mmapfptr; // To store the starting address of the memory mapped file
int mmapfile_fd = open("BACKING_STORE.bin", O_RDONLY);
if (mmapfile_fd == -1) {
      perror("Error opening file");
      exit(EXIT_FAILURE);
}

mmapfptr = mmap(NULL, LOGICAL_ADDRESS_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
```

The catch here is how will memcpy() work in our write_frame() function

So basically, memcpy's first argument is the destination file, which is the physical_memory's nested array based on the page number

For the second argument, which is important in this case, I was able to deduce something from the lab assignments

In the lab assignments we were copying from a memory mapped file to an array and the code line is as follows:

```c

memcpy(&intArray[i], mmapfptr + 4 * i, 4);
```

So the second argument here is `mmapfptr + 4 * i` which essentially means mmapfptr's current location + (where the next integer is located) because each integer takes 4 bytes and based on the iterator we are moving through each integer.

The third argument is 4 because we are copying each integer one by one we only read 4 bytes

In our case each signed char only takes 1 byte so we can leverage that. Whenever we want to copy a page from the mmapped file to the physical memory, we can get the page address by multiplying page number by page size ($page_number * 256$) and use it as the second argument to indicate the point from where we want to start reading (offset).

The third argument can be 256 since each signed char takes up 256 bytes and our nested array size is also 256 due to the fact it's made up of signed chars we can just replace it by copying 256 bytes.

## Requirement 2: Inclusion of TLB

TLB can be implemented as a circular array, which each value being a custom data type which stores frame_number and page_number.

The custom data type TLBentry is as follows:-

```c
typedef struct {
  int page_number;
  int frame_number;
} TLBentry;
```

Similar to physical memory we will create an `int tlb_next_index = 0` to preserve circular buffer logic

We will initialize our array:

```c
TLBentry tlb[TLB_SIZE]
```

For searching the TLB for a value, as described in the lectures, we just manually search every entry to check if our page_number is there or not.

```c
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

```

For adding a page and a frame number, we just map the values of page number and frame number to the corresponding members of the struct type. In order to make sure that this is in accordance with the circular array principles (so that we overwrite the oldest entry with the new one in case of an overflow), we get the remainder after division with the max TLB size (remainder cannot be greater than the divisor => new values will always be appended to the circular buffer and will overwrite the oldest values).

```c
void add_TLB(int page_number, int frame_number)
{
  TLB[TLBindex].page_number = page_number;
  TLB[TLBindex].frame_number = frame_number;
  TLBindex = (TLBindex + 1) % TLB_SIZE;

  if (TLBcount < TLB_SIZE)
    TLBcount++;
}
```

We will also update the TLB in case some page is replaced in the physical memory. This is done to avoid faulty hits. If our physical memory got overwritten and that page doesn't exist in it, then we have to overwrite it in our TLB as well otherwise it will return a wrong frame number which has been overwritten.

```c
void update_TLB(int page_number, int frame_number)
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
}
```

## Incorporating TLB logic in our original code

Alongside page_table, we would be initializing the frame number with -1 as well (if a page doesn't exist, it will be denoted by -1).

```c
for (int i = 0; i < TLB_SIZE; i++)
{
  TLB[i].page_number = -1;
  TLB[i].frame_number = -1;
}
```

We initialize an `int TLB_hits` variable so that we can keep track of total TLB hits. We will update it's value whenever search_TLB(page_number) doesn't return -1.

Additionally, in the case neither TLB nor the page_table has the frame_number, we will update an `int page_fault` variable.

In the case that the TLB missed the frame_number and it's not in the memory as well, we would be manually adding the page to the physical_memory using the replace_page function. At the same time we would add it to the TLB as well for future cache hits in order to optimize performance.

Also while replacing the page, we update our TLB. If the page_number hasn't been replaced by some new frame, the tlb_update occurs, but doesn't change anything. In the case that our physical memory was full and in order to add a new page from logical address, it replaced the oldest address, the update_tlb will make sure that our tlb data structure avoids faulty hits.

## Prerequisite Knowledge

- Reading a file in C (Refer to lab 4 part 1)
- Converting logical addresses to physical addresses (refer to lab 4 part 1)
- Usage of mmap() and memcpy() to map a file in the local storage to memory so that it can be used as some sort of a virtual memory (refer to lab 4 part 2).
- Understanding of paging and TLB's (refer to chapter 9 lec notes).
