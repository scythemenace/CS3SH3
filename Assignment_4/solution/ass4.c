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

int main() {
    // Open addresses.txt in read mode
    FILE *file = fopen("sample.txt", "r");
    if (file == NULL) {
        perror("Error opening addresses.txt");
        return 1;
    }

    // Initialize the page table with all entries set to -1
    // -1 indicates that the page is not currently loaded into physical memory
    int page_table[PAGE_TABLE_SIZE];
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i] = -1;
    }

    // Simulate physical memory as a 2D array
    // Each row represents a frame that can hold one page of FRAME_SIZE (256) bytes
    signed char physical_memory[NUM_FRAMES][FRAME_SIZE];

    int next_frame = 0;   // Keeps track of the next available frame in physical memory
    int page_faults = 0;  // Counter for the total number of page faults

    char line[20];  // Buffer to hold each line read from the input file. 20 is chosen at random

    // Read each logical address from the file
    while (fgets(line, 20, file)) { // 20 is the soze of line
        // Convert the string to an unsigned 16-bit integer. We are using uint16_t datatype for 16 bit address
        uint16_t logical_address = (uint16_t)atoi(line);

        // Extract the page number and offset using bitwise operations
        int page_number = (logical_address & PAGE_MASK) >> SHIFT;
        int offset = logical_address & OFFSET_MASK;

        int frame_number;  // Will hold the frame number where the page is located

        // Check if the page is already in the page table
        if (page_table[page_number] != -1) {
            // Page is in memory; retrieve the frame number from the page table
            frame_number = page_table[page_number];
        } else {
            // Page fault occurs because the page is not in memory
            page_faults++;

            // Assign the next available frame to the page
            frame_number = next_frame;
            next_frame++;

            // Update the page table with the new page-to-frame mapping
            page_table[page_number] = frame_number;

            // Simulate loading the page into physical memory
            // For this part, we'll fill the frame with the page number for simplicity
            memset(physical_memory[frame_number], page_number, FRAME_SIZE);
        }

        // Compute the physical address using the frame number and offset
        int physical_address = (frame_number << SHIFT) | offset;

        // Retrieve the value stored at the physical address in physical memory
        signed char value = physical_memory[frame_number][offset];

        // Output the logical address, physical address, and the value at that address
        printf("Logical Address: %u\n", logical_address);
        printf("Physical Address: %d\n", physical_address);
        printf("Value: %d\n", value);
    }

    // Close the file after processing all addresses
    fclose(file);

    // Output the total number of page faults encountered during address translation
    printf("Number of Page Faults: %d\n", page_faults);

    return 0;
}