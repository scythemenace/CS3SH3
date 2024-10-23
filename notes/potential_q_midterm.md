# Potential Question for CS3SH3 Midterm 1

## Chapter 3

### Draw the process tree

**Example question** - Draw the process for the code given below and indicate pid1 and pid2 for each process created.

Code is as follows:-

```c
int main() {
    pid_t pid1, pid2;
    pid1 = fork();  // First fork
    pid2 = fork();  // Second fork

    if (pid1 == 0) {
        fork();     // Third fork
    }
    if (pid2 == 0) {
        fork();     // Fourth fork
    }

    return 0;
}
```

The process tree for the code above is as follows:-
![Process Tree](./potential_q_images/process_tree.png)

Explanation

1. First fork() (pid1):
   - P1 creates P2.
   - Now we have two processes: P1 and P2.
2. Second fork() (pid2):
   - Both P1 and P2 execute this fork().

- P1 creates P3.
  - P2 creates P4.
  - Now we have four processes: P1, P2, P3, and P4.

3.  Third fork() (inside if (pid1 == 0)):
    - Only processes where pid1 == 0 (P2 and its descendants) execute this.
    - P2 creates P5.
    - P4 creates P6.
    - Now we have six processes: P1, P2, P3, P4, P5, and P6.
4.  Fourth fork() (inside if (pid2 == 0)):
    - Only processes where pid2 == 0 (P3 and P4) execute this.
    - P3 creates P9.
    - P4 creates P8.
    - Now we have nine processes: P1, P2, P3, P4, P5, P6, P8, and P9.
