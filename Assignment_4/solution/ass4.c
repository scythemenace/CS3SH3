#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>    // For open()
#include <unistd.h>   // For close()
#include <sys/mman.h> // For mmap()

// Constants for address masking and shifting
#define PAGE_MASK 0xFF00       // Mask to extract the page number
#define OFFSET_MASK 0x00FF     // Mask to extract the offset
#define SHIFT 8                // Number of bits to shift for page number

#define PAGE_TABLE_SIZE 256    // Total number of pages (logical address space / page size)
#define FRAME_SIZE 256         // Size of each frame/page in bytes
#define NUM_FRAMES 128         // Total number of frames (physical address space / page size)

#define TLB_SIZE 16            // Maximum number of entries in the TLB

// TLB Entry structure
typedef struct {
    int page_number;
    int frame_number;
} TLBentry;

// Global variables
int page_table[PAGE_TABLE_SIZE];
signed char physical_memory[NUM_FRAMES][FRAME_SIZE];
TLBentry TLB[TLB_SIZE];
int TLB_index = 0;           // Points to the next TLB entry to replace (oldest entry)
int next_frame = 0;          // Keeps track of the next available frame
int page_faults = 0;         // Counter for page faults
int TLB_hits = 0;            // Counter for TLB hits
int total_addresses = 0;     // Counter for total addresses processed

// Variables for FIFO page replacement
int page_order[NUM_FRAMES];  // Keeps track of the order of pages in frames
int frame_index = 0;         // Points to the next frame to replace

// Backing store variables
int backing_store_fd;        // File descriptor for the backing store
signed char *backing_store;  // Pointer to the mapped backing store



// Function to search the TLB for a given page number
int search_TLB(int page_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (TLB[i].page_number == page_number) {
            // TLB hit
            return TLB[i].frame_number;
        }
    }
    // TLB miss
    return -1;
}

// Function to add a page-to-frame mapping to the TLB
void TLB_Add(int page_number, int frame_number) {
    // Add the new entry at the current TLB_index position
    TLB[TLB_index].page_number = page_number;
    TLB[TLB_index].frame_number = frame_number;

    // Update the TLB_index to point to the next entry
    TLB_index = (TLB_index + 1) % TLB_SIZE;
}

// Function to update the TLB when a page is replaced
void TLB_Update(int replaced_page_number, int new_page_number, int frame_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (TLB[i].page_number == replaced_page_number) {
            // // Invalidate the TLB entry for the replaced page
            // TLB[i].page_number = -1;
            // TLB[i].frame_number = -1;


            // Overwrite the TLB entry with the new page
            TLB[i].page_number = new_page_number;
            TLB[i].frame_number = frame_number;
            break;
        }
    }
    // Add the new page-to-frame mapping to the TLB
    TLB_Add(new_page_number, frame_number);
}



int main() {
    // Initialize the page table with all entries set to -1
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i] = -1;
    }

    // Initialize the TLB entries to -1
    for (int i = 0; i < TLB_SIZE; i++) {
        TLB[i].page_number = -1;
        TLB[i].frame_number = -1;
    }

    // Initialize the page_order array
    for (int i = 0; i < NUM_FRAMES; i++) {
        page_order[i] = -1;
    }



    // Open the backing store file
    backing_store_fd = open("../BACKING_STORE.bin", O_RDONLY);
    if (backing_store_fd == -1) {
        perror("Error opening BACKING_STORE.bin");
        return 1;
    }

    // Map the backing store into memory
    backing_store = mmap(0, 65536, PROT_READ, MAP_PRIVATE, backing_store_fd, 0);
    if (backing_store == MAP_FAILED) {
        perror("Error mapping BACKING_STORE.bin");
        close(backing_store_fd);
        return 1;
    }
    // Close the backing store file descriptor
    close(backing_store_fd);



    // Open the file containing logical addresses
    FILE *file = fopen("../addresses.txt", "r");
    if (file == NULL) {
        perror("Error opening addresses.txt");
        munmap(backing_store, 65536);
        return 1;
    }

    char line[20];  // Buffer to hold each line from the file

    // Read each logical address from the file
    while (fgets(line, 20, file)) { // 20 is the size of the line
        total_addresses++;

        // Convert the logical address to an unsigned 16-bit integer. Using uint16_t here to store 16 bit values
        uint16_t logical_address = (uint16_t)atoi(line);

        // Extract the page number and offset
        int page_number = (logical_address & PAGE_MASK) >> SHIFT;
        int offset = logical_address & OFFSET_MASK;

        int frame_number;

        // Step 1: Check the TLB for the page number
        frame_number = search_TLB(page_number);
        if (frame_number != -1) {
            // TLB hit
            TLB_hits++;
        } else {
            // TLB miss
            // Step 2: Check the page table
            if (page_table[page_number] != -1) {
                // Page is in memory
                frame_number = page_table[page_number];

                // Since we had a TLB miss but the page is in memory, we add it to the TLB
                TLB_Add(page_number, frame_number);
                
            } else {
                // Page fault occurs
                page_faults++;

                // Load the page from the backing store
                if (next_frame < NUM_FRAMES) {
                    // Free frame available
                    frame_number = next_frame;
                    next_frame++;

                    // Copy the page from backing store to physical memory
                    memcpy(physical_memory[frame_number], backing_store + page_number * FRAME_SIZE, FRAME_SIZE);

                    // Update the page table with the new page
                    page_table[page_number] = frame_number;

                    // Add the page to the page_order array
                    page_order[next_frame - 1] = page_number;

                    // Since we have loaded a new page into memory, add it to the TLB
                    TLB_Add(page_number, frame_number);
                } else {
                    // Physical memory is full, replace the oldest page using FIFO
                    frame_number = frame_index;

                    // Find the page currently in the frame to be replaced
                    int replaced_page = page_order[frame_index];

                    // Update the page table to invalidate the replaced page
                    page_table[replaced_page] = -1;

                    // Copy the page from backing store to physical memory
                    memcpy(physical_memory[frame_number], backing_store + page_number * FRAME_SIZE, FRAME_SIZE);

                    // Update the page table with the new page
                    page_table[page_number] = frame_number;

                    // Update the page_order array with the new page
                    page_order[frame_index] = page_number;

                    // Update the TLB
                    TLB_Update(replaced_page, page_number, frame_number);

                    // Move the frame_index to the next frame (circularly)
                    frame_index = (frame_index + 1) % NUM_FRAMES;
                }
            }


            // // Step 3: Add the page-to-frame mapping to the TLB
            // Do not call TLB_Add here since TLB_Update already handles adding the new page. // Fixed in second edit
            // TLB_Add(page_number, frame_number);
        }

        // Compute the physical address
        int physical_address = (frame_number << SHIFT) | offset;

        // Retrieve the value at the physical address
        signed char value = physical_memory[frame_number][offset];

        // Output the results
        printf("Virtual address: %u ", logical_address);
        printf("Physical address = %d ", physical_address);
        printf("Value=%d\n", value);
    }

    // Close the addresses file
    fclose(file);

    // Unmap the backing store
    if (munmap(backing_store, 65536) == -1) {
        perror("Error unmapping BACKING_STORE.bin");
        return 1;
    }

    // Output statistics
    printf("Number of Translated Addresses = %d\n", total_addresses);
    printf("Page Faults = %d\n", page_faults);
    printf("TLB Hits = %d\n", TLB_hits);

    return 0;
}