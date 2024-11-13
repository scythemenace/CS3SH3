#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define INT_SIZE 4
#define INT_COUNT 10
#define MEMORY_SIZE (INT_SIZE * INT_COUNT)

int main() {
    int intArray[INT_COUNT];
    signed char *mmapfptr;

    int mmapfile_fd = open("numbers.bin", O_RDONLY);
    if (mmapfile_fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    mmapfptr = mmap(NULL, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);

    for (int i = 0; i < INT_COUNT; i++) {
        memcpy(&intArray[i], mmapfptr + INT_SIZE * i, INT_SIZE);
    }

    munmap(mmapfptr, MEMORY_SIZE)

    close(mmapfile_fd);

    int sum = 0;
    for (int i = 0; i < INT_COUNT; i++) {
        sum += intArray[i];
    }
    printf("Sum of numbers: %d\n", sum);

    return 0;
}
