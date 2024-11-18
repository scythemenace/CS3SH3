#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
            // Overwrite the entry with the new page
            TLB[i].page_number = new_page_number;
            TLB[i].frame_number = frame_number;
            return;
        }
    }
    // If the replaced page was not in the TLB, add the new entry
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

    // Open the file containing logical addresses
    FILE *file = fopen("../addresses.txt", "r");
    if (file == NULL) {
        perror("Error opening addresses.txt");
        return 1;
    }

    char line[20];  // Buffer to hold each line from the file

    // Read each logical address from the file
    while (fgets(line, sizeof(line), file)) {
        total_addresses++;

        // Convert the logical address to an unsigned 16-bit integer
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
            } else {
                // Page fault occurs
                page_faults++;

                // Handle page fault (load page into memory)
                if (next_frame < NUM_FRAMES) {
                    // Free frame available
                    frame_number = next_frame;
                    next_frame++;
                } else {
                    // Physical memory is full, need to replace a page using FIFO policy
                    // For simplification, we replace the page in frame 0
                    frame_number = 0;

                    // Find the page currently in frame 0 and invalidate it
                    int replaced_page = -1;
                    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                        if (page_table[i] == frame_number) {
                            page_table[i] = -1;
                            replaced_page = i;
                            break;
                        }
                    }

                    // Update the TLB to reflect the change
                    TLB_Update(replaced_page, page_number, frame_number);
                }

                // Load the page into physical memory (simulated)
                page_table[page_number] = frame_number;
                memset(physical_memory[frame_number], page_number, FRAME_SIZE);
            }

            // Step 3: Add the page-to-frame mapping to the TLB
            TLB_Add(page_number, frame_number);
        }

        // Compute the physical address
        int physical_address = (frame_number << SHIFT) | offset;

        // Retrieve the value at the physical address
        signed char value = physical_memory[frame_number][offset];

        // Output the results
        printf("Logical Address: %u\n", logical_address);
        printf("Physical Address: %d\n", physical_address);
        printf("Value: %d\n", value);
    }

    // Close the file
    fclose(file);

    
    printf("Number of Translated Addresses = %d\n", total_addresses);
    printf("Page Faults = %d\n", page_faults);
    printf("TLB Hits = %d\n", TLB_hits);

    return 0;
}