#include <stdio.h>
#include <sys/types.h>

int main () {
  // P1
  pid_t pid;
  printf("%d\n", getpid());
  pid = fork();
  if (pid < 0) {
    printf("Fork failed");
  } else if (pid == 0) {
    // P2
    printf("%d\n", getpid());
    pid_t pid1;
    pid1 = fork();

    if (pid1 < 0) {
      printf("Fork failed");
    } else if (pid1 == 0) {
      // P9
      printf("%d\n", getpid());
    } else {
      while(waitpid(-1, NULL, 0) > 0);
      pid_t pid2;
      pid2 = fork();
      if (pid2 < 0) {
        printf("Fork failed");
      } else if (pid2 == 0) {
        // P3
        printf("%d\n", getpid());
        pid_t pid3;
        pid3 = fork();
        if (pid3 < 0) {
          printf("Fork failed");
        } else if (pid3 == 0) {
          // P7
          printf("%d\n", getpid());
        } else {
          // P3
          while(waitpid(-1, NULL, 0) > 0);
          pid_t pid4;
          pid4 = fork();
          if (pid4 < 0) {
            printf("Fork failed");
          } else if (pid4 == 0) {
            // P4
            printf("%d\n", getpid());
            pid_t pid5;
            pid5 = fork();
            if (pid5 < 0) {
              printf("Fork failed");
            } else if (pid5 == 0) {
              // P6
              printf("%d\n", getpid());
            } else {
              while(waitpid(-1, NULL, 0) > 0);
            }
          } else {
            while(waitpid(-1, NULL, 0) > 0);
          }
        }
      } else {
        //  P2
        while(waitpid(-1, NULL, 0) > 0);
        pid_t pid6;
        pid6 = fork();
        if (pid6 < 0) {
          printf("Fork failed");
        } else if (pid6 == 0) {
          // P5
          printf("%d\n", getpid());
          pid_t pid7;
          pid7 = fork();
          if (pid7 < 0) {
            printf("Fork failed");
          } else if (pid7 == 0) {
            // P8
            printf("%d\n", getpid());
          } else {
            while(waitpid(-1, NULL, 0) > 0);
          }
        } else {
          while(waitpid(-1, NULL, 0) > 0);
        }
      } 
    }

  } else {
    while(waitpid(-1, NULL, 0) > 0);
  }
 
  return 0;
}
