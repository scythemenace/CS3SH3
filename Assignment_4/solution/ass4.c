#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PAGE_MASK 0xFF00
#define OFFSET_MASK 0x00FF
#define SHIFT 8

int main() {
    FILE *file = fopen("sample.txt", "r");
    if (file == NULL) {
        perror("Error opening addresses.txt");
        return 1;
    }

    char line[20];
    while (fgets(line, sizeof(line), file)) {
        uint16_t logical_address = (uint16_t)atoi(line);
        int page_number = (logical_address & PAGE_MASK) >> SHIFT;
        int offset = logical_address & OFFSET_MASK;

        printf("Logical Address: %u\n", logical_address);
        printf("Page Number: %d, Offset: %d\n", page_number, offset);
    }

    fclose(file);
    return 0;
}

