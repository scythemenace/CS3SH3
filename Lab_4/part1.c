#include <stdio.h>
#include <stdlib.h>

//As stated, the logical addresses are no more than size 10
#define BUFFER_SIZE 10

int main() {
  FILE *fptr = fopen("labaddr.text", "r");
  if (fptr == NULL) {
    fprintf(stderr, "Error opening file.\n");
    return -1;
  }

  char buff[BUFFER_SIZE];
  while (fgets(buff, BUFFER_SIZE, fptr) != NULL) {
    printf("Read logical address: %s", buff);
  }

  fclose(fptr);
  
  return 0;
}
