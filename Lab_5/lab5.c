#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

struct stat fileAttributes;

void printFilePermissions(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &fileAttributes) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("a. Inode: %ju\n", (uintmax_t)fileAttributes.st_ino);
    printf("b. Size (in bytes): %jd bytes\n", (intmax_t)fileAttributes.st_size);
    printf("c. Blocks: %jd\n", (intmax_t)fileAttributes.st_blocks);
    printf("d. File Permissions: %#o/", fileAttributes.st_mode & 0777);
    printFilePermissions(fileAttributes.st_mode);
    printf("e. Uid: %u\n", fileAttributes.st_uid);
    printf("f. Time of last access: %s", ctime(&fileAttributes.st_atime));
    printf("g. Time of data modification: %s", ctime(&fileAttributes.st_mtime));
    printf("h. Last status change time: %s", ctime(&fileAttributes.st_ctime));

    exit(EXIT_SUCCESS);
}

