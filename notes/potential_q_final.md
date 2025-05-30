# Potential Questions for CS3SH3 Final

<!--toc:start-->

- [Potential Questions for CS3SH3 Final](#potential-questions-for-cs3sh3-final)
  - [Chapter 6](#chapter-6)
    - [What is a race condition?](#what-is-a-race-condition)
    - [What is the producer-consumer problem? How can an improper implementation result in a race condition?](#what-is-the-producer-consumer-problem-how-can-an-improper-implementation-result-in-a-race-condition)
    - [What is the critical section (CS) problem?](#what-is-the-critical-section-cs-problem)
    - [Dining philosopher](#dining-philosopher)
    - [Conditions for a solution to satisfy a CS Problem](#conditions-for-a-solution-to-satisfy-a-cs-problem)
    - [Explain that Peterson's solution is correct](#explain-that-petersons-solution-is-correct)
    - [What are the drawbacks of Peterson's solution](#what-are-the-drawbacks-of-petersons-solution)
    - [What is Preemption? Explain the two approaches taken by the kernel to handle CS?](#what-is-preemption-explain-the-two-approaches-taken-by-the-kernel-to-handle-cs)
    - [Conditional Variables](#conditional-variables)
    - [Bounded Buffer Hypothetical Deadlock](#bounded-buffer-hypothetical-deadlock)
    - [Explain why interrupts are not appropriate for implementing synchronization primitives in multiprocessor systems.](#explain-why-interrupts-are-not-appropriate-for-implementing-synchronization-primitives-in-multiprocessor-systems)
    - [Memory Barrier Usage](#memory-barrier-usage)
    - [test_and_set(\*target) and compare_and_swap(\*value, expected, new_value)](#testandsettarget-and-compareandswapvalue-expected-newvalue)
  - [Chapter 3](#chapter-3)
    - [What is Context Switching?](#what-is-context-switching)
    - [What are the two types of processes?](#what-are-the-two-types-of-processes)
    - [What is a process control block (PCB)](#what-is-a-process-control-block-pcb)
    - [IPC, Two Models and Where To Use One](#ipc-two-models-and-where-to-use-one)
    - [Draw the process tree](#draw-the-process-tree)
    - [How to query the process id of the current process and the parent process?](#how-to-query-the-process-id-of-the-current-process-and-the-parent-process)
    - [What does the exec() call do?](#what-does-the-exec-call-do)
    - [What is cascading termination?](#what-is-cascading-termination)
    - [What is a zombie process and an orphan process?](#what-is-a-zombie-process-and-an-orphan-process)
  - [Chapter 4](#chapter-4)
    - [What is the advantage of using threads](#what-is-the-advantage-of-using-threads)
    - [Give three examples of data parallelism and task parallelism each](#give-three-examples-of-data-parallelism-and-task-parallelism-each)
    - [Ahmdal's Law](#ahmdals-law)
    - [What is many-to-one kernel thread mapping](#what-is-many-to-one-kernel-thread-mapping)
    - [What is one-to-one kernel thread mapping](#what-is-one-to-one-kernel-thread-mapping)
    - [What is many-to-many kernel thread mapping](#what-is-many-to-many-kernel-thread-mapping)
    - [What is two level kernel thread mapping](#what-is-two-level-kernel-thread-mapping)
    - [Performance Implications of different scenarios in a many-to-many Model](#performance-implications-of-different-scenarios-in-a-many-to-many-model)
    - [What is the output of the Line "xyz" - Thread based](#what-is-the-output-of-the-line-xyz-thread-based)
  - [Chapter 5](#chapter-5) - [What is the difference between IO Bound program and CPU bound program](#what-is-the-difference-between-io-bound-program-and-cpu-bound-program) - [What are the different scheduling queues that an operating system has? Which queue is accessible by the CPU scheduler?](#what-are-the-different-scheduling-queues-that-an-operating-system-has-which-queue-is-accessible-by-the-cpu-scheduler) - [What is the use of a CPU scheduler?](#what-is-the-use-of-a-cpu-scheduler) - [What could be a reason to have Preemptive scheduling (stopping a running process)?](#what-could-be-a-reason-to-have-preemptive-scheduling-stopping-a-running-process) - [What's the use of a Dispatcher Module](#whats-the-use-of-a-dispatcher-module) - [What is the difference between dispatch latency and context switching](#what-is-the-difference-between-dispatch-latency-and-context-switching)
  <!--toc:end-->

## Chapter 6

### What is a race condition?

Race condition refers to a situation where several threads try to access and modify shared data concurrently.

### What is the producer-consumer problem? How can an improper implementation result in a race condition?

A producer-consumer problem is a synchronization problem where there exists a buffer of some size and a producer adds to the buffer and a consumer subtracts from the buffer. There are three things we need to keep in mind:-

- If the buffer is full, then the producer cannot add to it. Therefore, it must wait until the consumer consumes some data and then it starts adding on to it again.
- If the buffer is empty, then the consumer cannot consume from it. Therefore, it must wait until the producer adds some data to it and then it can start consuming it again.
- Consumption and Production of data shouldn't happen at the same time as it can lead to data inconsistency.

The last point essentially means that let's say we are keeping track of a variable called counter. The counter represents the buffer and it cannot be greater than the buffer size. Initially the counter is 0, so the consumer waits and then the producer adds on to it. In the case it reaches the buffer's max value, the producer waits until the consumer consumes it. The problem that can happen here is that let's say both the producer and consumer start changing the counter variable's value.

Normally producer adds one to the counter to simulate data being added and the consumer subtracts one from the data to simulate data being consumed. If they both do that at the same time it's very much possible that the counter value might not be what we expect it to be since one change can overwrite the other.

For example, let's say the counter is 5 and the producer added on to it using a register, the value becomes 6 but the register has not overwritten the value of the actual counter yet. Let's say the consumer subtracts one from counter using a register but also has still not overwritten the value. Let's say producer is the first one to overwrite and counter becomes 6, then let's say producer also overwrites it and the counter becomes 4. This is not the intended behaviour since the value of counter should've been 5 due to the fact that producer added 1 to it and consumer subtracted one from it.

### What is the critical section (CS) problem?

Each process has its own CS section. This is created to avoid race conditions by making sure that only one process can access the shared data at a time.

The general structure of a CS section is as follows:

```c
do {

  // Beginning section

  // Critical Section

  // Exits CS

  // Remainder Section

} while(true);
```

### Dining philosopher

There are 5 philosophers sitting in a circle and alternate between thinking and eating. Each philosopher is a neighbour to other philosophers but they don't interact with each other.

There are 5 chopsticks and a bowl of rice (shared resource - assumed infinite for simplicity). Philosophers use the chopsticks and eat whenever they are not thinking.

![Dining Philosophers](./potential_q_images/dining-phil.png)

The following code describes the process:-

```c
do {
    wait(chopstick[i]);  // Pick up the right chopstick
    wait (chopstick[(i + 1) % 5]); // Pick up the left chopstick

    // eat

    signal(chopstick[i]);
    signal(chopstick[(i + 1) % 5])
}
```

A potential problem in this code is that in the case all threads(philosophers) start executing(eating) at the same time at the same, they all might pick up the right chopstick since that's the first line of the code. However for a random philosopher someone's right chopstick could be their left chopstick since they are sitting in a circle. This would lead to a deadlock since each process(philosopher) is waiting for the other to release their right chopstick so that they can acquire it as their left chopstick and start eating.

There are three potential solutions for solving this problem:-

- At most only 4 philosophers are allowed to sit
- Asymmetric solution: Rewrite the code such that odd numbered philosopher picks left chopstick first whereas even numbered philosopher picks right chopstick first
- A philosopher is only allowed to pick up a chopstick if both chopsticks besides him are available. In the case that even one of the chopsticks (either right or left) is not available, he can't pick up any of them. We can simulate this behaviour by blocking the part of the code where we pick up chopsticks in a mutex lock. This ensures whenever some other philosophers are trying to acquire the chopsticks, the other philosophers can't access it at the same time.

### Conditions for a solution to satisfy a CS Problem

- **Mutual Exclusion**: If there are two processes $P_{\text{i}}$, the Critical Section can only be accessed by one and only one process at a time.
- **Progress**: Only those processes which is not executing in its remainder section must have an equal chance of entering its CS. The selection of process cannot arbitrarily be delayed indefinitely.
- **Bounded Waiting**: If a particular process is waiting to enter its CS, there must be a finite limit which allows other processes to enter their CS before the processes' request is granted.

### Explain that Peterson's solution is correct

- **Mutual Exclusion**: Even if both flags[0] and flags[1] are true at the same time, the turn variable is allocated the value of either 0 or 1 based on the order of execution. Since the while loop condition in the code is dependant on the latest value of turn, only one processes is allowed to enter it's critical section.
- **Progress**: Both processes have an equal chance of getting into their critical section. Even though both flags can be true at the same time, there is a random chance of turn to be 0 or 1 based on the order of execution. Additionally, the remainder section doesn't interfere with the selection of a process to go into its critical section. The selection of a process to go into its CS is done in finite time.
- **Bounded Waiting**: We know the variable turn determines which progress will go into its CS first. Let's assume both flags are true, depending on turn any process can go into its CS. Until its done executing, the other processes waits. After the process is done executing it sets its flag as false which allows the other process to go into its CS. Therefore each process waits for the other process to execute at most once.

### What are the drawbacks of Peterson's solution

The drawbacks of Peterson's solution are:-

- Only works for two processes
- Doesn't work on modern hardware due to vagaries of load and store operations

### What is Preemption? Explain the two approaches taken by the kernel to handle CS?

Preemption means temporarily stopping the execution of a process after it has executed for a specific period of time and bringing another program to execute in its place.

Two approaches taken by the kernel to handle CS are:-

- **Preemptive**: The kernel allows a process in its CS to be preempted while running in kernel mode.
- **Non Preemptive**: The kernel doesn't allow the process to stop its execution until it exists kernel mode, blocks, or voluntarily gives up access to CPU.

### Conditional Variables

- Conditional Variables are only used in monitors unlike Semaphores which can be used anywhere.
- Monitors allow only process to run at a time. If let's say process Q was waiting on a condition (x.wait() where x is a conditional variable) and another process P released the wait by invoking x.signal(), since only one process can run at a time, based on the implementation of monitors, either P **signals and waits** i.e. it removes the wait from process Q and let's Q run until it leaves the monitor or waits on another conditional variable (let's say y: y.wait()). Another choice could be P **signals and continues** meaning it uplifts the wait on Q but makes it wait until it leaves the monitor or waits on another conditional variable (opposite of signal and wait). Don't get confused by the fact that they have to wait even though signaling has been done. Signaling uplifts a wait imposed by a conditional variable's wait() call. Whereas **signal and wait** and **signal and continue** is done in other to ensure no two processes can be executed at the same time.
- This is the key difference between Semaphores and Conditional Variables. Semaphores, don't necessarily always block the processes which call them since sometimes wait() can just reduce the value of a semaphore counter whereas conditional variables always block the processes that call them (.wait() and .signal() -> makes the decision either **signal and wait** or **signal and continue** ). You can think of semaphores being a counter for synchronization tasks whereas conditional variables operate solely based on certain conditions.
- If several processes are waiting on the same conditional variable x by invoking x.wait(), x.signal() wakes up a process based on First Come First Serve basis. Another option is to **conditional-wait** where we give a wait call with a priority. The lower the priority, the earlier the wakeup.

### Bounded Buffer Hypothetical Deadlock

The code of producer is as follows:-

```c
do {
/* produce an item in next_produced */
wait(empty);
wait(mutex);
/* add next produced to the buffer */
signal(mutex);
signal(full);
} while (true);
```

The code of consumer is as follows:-

```c
do {
wait(full);
wait(mutex);
/* remove an item from buffer to next_consumed */
signal(mutex);
signal(empty);
/* consume the item in next consumed */
} while (true);
```

**Question that can be asked**: What if we switch signal(empty) and signal(mutex) with each other? How will it impact the code?
If we switch the line signal(mutex) and signal(empty), it could potentially lead to a deadlock.
Switched code will be:-

```c
do {
/* produce an item in next_produced */
wait(mutex);
wait(empty);
/* add next produced to the buffer */
signal(mutex);
signal(full);
} while (true);
```

Imagine a scenario where the buffer is full i.e. empty = 0. In this case if our code acquires the mutex lock first, it will run into wait(empty). Since empty is 0, it just waits there and never releases the lock. The consumer process also can't do anything since it's waiting for the lock to be released in order to consume the buffer and due to the lock never releasing, it gets stuck in a deadlock.

### Explain why interrupts are not appropriate for implementing synchronization primitives in multiprocessor systems.

Interrupts are not atomic and also have a lot of overhead due to which it shouldn't be used to implement synchronization primitives

### Memory Barrier Usage

Memory barries are used because sometimes computers can reorder instructions which can cause discrepencies in our code.
One such situation is as follows:-

Thread 1:

```c
// Thread #1 is waiting in a loop until f becomes 1.
// Once f is set to 1, Thread #1 will exit the loop and print the value of x
while (f == 0);
print x;
```

Thread 2:

```c
// Thread #2 sets x = 42.
// Then, Thread #2 sets f = 1, signaling to Thread #1 that the value of x is ready to be read.
x = 42;
f = 1;
```

**Intended Behaviour**

- Thread 2 should first set the value of x and then set the value of f as 1 in order to break the loop in thread 1
- Thread 1 waits until f equals 1 and then reads the value of x.
- The expected result is that thread 1 prints 42, once f becomes 1.

**The Problem**

- If the instruction `x = 42` and `f = 1` gets reordered then f might become 1 before x is set to 42. If thread 1 executes immediately after f becomes 1 then it might print an incorrect value of x which was not intended.

Therefore a memory barrier is needed. The corrected code is as follows:-

Thread 1:

```c
while (f == 0);
memory_barrier();  // Ensures that f == 1 is fully observed before reading x
print x;           // Now reads the correct value of x (42)
```

Thread 2:

```c
x = 42;
memory_barrier();  // Ensures x = 42 is committed before f = 1
f = 1;
```

Memory barriers force any changes that you want up to the point its called to be forcefully propagated to all systems (like a github commit). So x = 42 will be known before f becomes 1.

### test_and_set(\*target) and compare_and_swap(\*value, expected, new_value)

test_and_set() returns the target's value but replaces it with true. This is useful for scenarios like the bounded buffer.

```c
bool test_and_set(bool *target) {
  bool old_val = *target;
  *target = true;
  return old_val
}
```

This is useful when we have to implement a mutex lock using the test_and_set() atomic hardware instruction. Assume that the following structure is available:-

```c
typedef struct {
bool held;
} lock;
```

`held == false (true)` indicates that the lock is available (not available) Using the `struct lock`, illustrate how the following functions can be implemented using the test_and_set() instructions:

- a. `void acquire(lock \*mutex)`
- b. `void release(lock \*mutex)`

```c
void acquire(lock *mutex) {
  while(test_and_set(&mutex->held));
  // Do whatever
}

void release(lock *mutex) {
  &mutex->held == false;
}
```

compare_and_swap() gives us more flexibility to do that. We don't have to stick to just using true to replace the target, we can be more flexible here.

```c
int compare_and_swap(int *value, int expected, int new_val) {
  int old_val = *value;
  if (*value == expected) {
    *value = new_val
  }

  return old_val;
}
```

We can simulate the same functionality as test_and_set using compare_and_swap by giving it appropriate values. The code for the same acquire and release is given below using compare_and_swap();

```c
void acquire(lock *mutex) {
  while(compare_and_swap(&mutex->held, 0, 1));
  // Do whatever
}

void release(lock *mutex) {
  &mutex->held == 0;
}
```

## Chapter 3

### What is Context Switching?

Context Switching is done when the CPU wants to switch to a different process. CPU saves the context of the old process and loads the saved context for the new process.

### What are the two types of processes?

Process are either **IO Bound** or **CPU bound**. **IO Bound** processes have **many short CPU bursts** whereas **CPU Bound** have **few long CPU bursts**.

### What is a process control block (PCB)

A Process Control Block (PCB) stores all the information about a process. It contains information like state, counter, registers, etc.

### IPC, Two Models and Where To Use One

IPC stands for **Inter-Process Communication**. It refers to the mechanisms used by processes to exchange data and communicate with each other in an operating system. Since processes have separate memory spaces, they need IPC techniques to share information, coordinate their actions, or synchronize their behavior.

It has two models:-

- `Shared Memory`: Two or more processes communicate by accessing a common memory space. Processes can read or write to this memory area to exchange information.
- `Message Passing`: Processes communicate by sending and receiving messages, typically through the operating system. Difference between shared memory and message passing is that this model is simpler and safer (no need to worry about synchronization issues), but it may be slower than shared memory due to the overhead of message copying and system calls.

Shared memory is used when data has to be frequently transferred between processes without the overhead required for system calls. Typically in these cases a large amount of data is being transferred which is not very efficient using the message passing model.

Message Passing is useful when processes are loosely-coupled i.e. only need to exchange small messages. Additionally in certain systems where shared-memory isn't applicable (like distributed systems) message passing is the only option.

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

### How to query the process id of the current process and the parent process?

We can query the process id of the current process using `getpid()` and the parent process id using `getppid()`.

### What does the exec() call do?

The `exec()` call is used after fork() which allows the child process to execute a different program from the parent process. It replaces the current process image with a new one. It is also referred to as an **overlay**.

### What is cascading termination?

In order to avoid a child process to run after its parent has terminated, the OS automatically terminates all children, grandchildren, and so on when the parent process terminates. This is known as cascading termination.

### What is a zombie process and an orphan process?

A **zombie process** is a process which has terminated but has not been reaped (i.e. removed from the process table). This usually happens if the parent process didn't invoke `wait()`.

An **orphan process** is a process which has been terminated but its parent also terminated before reaping it (i.e. didn't invoke `wait()`). Basically a zombie process whose parent also terminated.

## Chapter 4

### What is the advantage of using threads

- Cheaper than process creation
- Resource sharing happens by default and is much simpler
- It is much easier to context switch using threads
- Threads enable concurrency (parallelism in multi core processors)

### Give three examples of data parallelism and task parallelism each

Data Parallelism means dividing the data into smaller parts and distributing it across multiple cores where the same operation is performed on different subsets of data

- Example:

  - Matrix Multiplication: When multiplying two matrices, different rows or blocks of the matrices can be multiplied independently, as the operations on different rows do not depend on each other. Each processor handles a chunk of the matrix data.

  - Image Processing: Applying a filter (e.g., blur, edge detection) to an image can be divided by splitting the image into smaller regions. Each region is processed independently by different processors.

  - Adding values in an Array: In an array of size N, if we have to add all the values inside it, we can distribute them into the number of cores.

Task Parallelism means distributing threads across different cores where each thread performs an action

- Example:

  - Web Server: A web server handling multiple client requests can assign different tasks (such as reading from a database, processing logic, or sending responses) to different threads or processors. Each task operates independently and can be processed in parallel.

  - Video Game Rendering: Different aspects of rendering a video game, such as calculating physics, AI decision-making, and rendering the graphics, can be handled by separate threads, as these tasks do not need to be done sequentially.

  - Editing word document: Different threads to interpret the keystrokes, display images, spelling and grammar, and perform automatic backups periodically.

### Ahmdal's Law

According to Amhdal's law, performance gain from increasing number of cores and making your application parallel is given by:-
$\cfrac{1}{S + \cfrac{1 - S}{N}}$

**An example question is**: Assume we have an application that is 70 percent parallel and 30 percent serial. If we
run this application on a system with 4 processing cores, what is the speed-up?

Answer would be $\cfrac{1}{0.3 + \cfrac{1 - 0.3}{4}}$ = $\cfrac{1 \times 4}{1.9}$ ~ 2.1 times faster

### What is many-to-one kernel thread mapping

- All user-level threads are mapped to one kernel thread.
- This is problematic because if one user thread blocks (maybe because it has to use an IO operations, etc.) it causes the whole process to be blocked (since all threads are blocked)

### What is one-to-one kernel thread mapping

- Each user-level threads are mapped to one kernel thread.
- This is overkill since this might have a lot of overhead because creating a user level thread creates a separate kernel level thread to manage it.

### What is many-to-many kernel thread mapping

- Allows many user-level threads to be mapped to an equal or less number of kernel level threads.
- Blocking kernel system doesn't block the entire process since the other kernel threads allotted to that batch of user-level threads facilitate execution

### What is two level kernel thread mapping

- Variation of the many-to-many kernel model (basically a combination of many-to-many and one-to-one).
- Allows a single user-level thread to be mapped to a single kernel thread and also facilitates whatever the many-to-many model does.

### Performance Implications of different scenarios in a many-to-many Model

Consider a multicore system and a multithreaded program written using the many-to-many threading model. Let the number of user-level threads in the program be greater than the number of processing cores in the system. Discuss the performance implications of the
following scenarios:

- The number of kernel threads allocated to the program is less than the number of processing cores.
- The number of kernel threads allocated to the program is equal to the number of processors.
- The number of kernel threads allocated to the program is greater than the number of processors.

**Answer**

- In the first case if the number of threads allocated are less than the number of processing cores, overall efficiency of the program will be worse due to the fact that core(s) will be underutilized. This may cause bottlenecks if the processing times are higher for some tasks which is unnecessary given the fact that few cores might be idle.
- In this case, each kernel thread can be scheduled on a separate processor which leads to full utilization of resources and maximizes parallel execution. Performance gains might be the highest in this case.
- The last case might be problematic due to the fact that the operating system has to context switch between the kernel threads to share processor time. Context switches come with a lot of overhead and no useful work is done which leads to poor performance and potential delays.

### What is the output of the Line "xyz" - Thread based

The program shown in Figure 4.16 (page 194 of the textbook) uses the Pthreads API. What would be the output from the program at LINE C and LINE P?

```c
#include <pthread.h>
#include <stdio.h>

int value = 0;
void *runner(void *param); /* the thread */

int main(int argc, char *argv[])
{
    pid_t pid;
    pthread_t tid;
    pthread_attr_t attr;

    pid = fork();

    if (pid == 0) { /* child process */
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, runner, NULL);
        pthread_join(tid, NULL);
        printf("CHILD: value = %d\n", value); /* LINE C */
    }
    else if (pid > 0) { /* parent process */
        wait(NULL);
        printf("PARENT: value = %d\n", value); /* LINE P */
    }
}

void *runner(void *param) {
    value = 5;
    pthread_exit(0);
}
```

Output of Line C would be **CHILD: value = 5** and Line P would be **PARENT: value = 0**. The reason is as follows:-

- When a child process is created, it gets its own memory space and all data from the parent is copied to the memory space.
- Initially the child has the value set as 0 which gets changed when the thread function is called by the thread created in the child process.
- The thread can only access the memory space of the process it was created in which is the child process' memory space. The value gets changed to 5 in the child process and gets printed since the thread gets collected using `pthread_join()` before the print statement.
- In the parent process code the value gets printed as 0 since previously the value was changed only in the child process' memory space. As we said before, the parent process has a different memory space, therefore, the value remains unchanged i.e. 0.

## Chapter 5

### What is the difference between IO Bound program and CPU bound program

An IO Bound program has **many** but **short** CPU bursts whereas a CPU bound has **few** but **longer** CPU bursts.

### What are the different scheduling queues that an operating system has? Which queue is accessible by the CPU scheduler?

An operating system has access to three queues:-

- Job Queue: Set of all processes in the system.
- Ready Queue: Set of all processes residing in the main memory waiting to be executed.
- Device Queue: Set of all processes waiting for an IO Device. There is a **separate device queue** for each device (for the most part).

Only the Ready Queue is accessibly by the CPU scheduler.

### What is the use of a CPU scheduler?

The CPU scheduler allocates the CPU to one of the processes in the ready queue. It doesn't follow any proper protocol for doing so since it's invoked frequently (milliseconds).

### What could be a reason to have Preemptive scheduling (stopping a running process)?

Preemptive scheduling interrupts a process and moves it back to the ready queue. A reason for this could be **time slicing approach**, where we give each process access to the CPU for a certain period of time in order to give the illusion that CPU belongs to all process (parallelism).

### What's the use of a Dispatcher Module

Dispatcher module gives control of the CPU to the process selected by the short term scheduler. It does this in three steps:-

- Switches context between the processes
- Switches to User Mode
- Jumps to proper location in the user program to restart that program.

### What is the difference between dispatch latency and context switching

Dispatch latency measures the time it takes for the dispatcher to stop one process and start another process whereas context switching is a **process** which saves the state of old process and restores the state of the new process.

## Chapter 8

### What is compaction?

Compaction shuffles memory contents to place all free memory spaces together. This is done to avoid external fragmentation.

## Chapter 10

## What is thrashing? Why does it decrease the throughput of a system?

Thrashing is a situation in which a process didn't get enough frames to support its active processes. Therefore it keeps on swapping pages which degrades performance. Basically it gets a page fault and replaces the page in memory, but soon enough gets another page fault. This process happens so frequently that processes wait for pages to get swapped therefore reducing CPU utilization greatly.

The OS thinks this is a problem of lack of multiprogramming, therefore it increases the degree of multiprogramming which in turn increases the number of processes in the system. This leads to more page faults and more swapping which leads to a cycle of thrashing. This decreases the throughput of the system since the CPU is not doing any useful work.

This can be avoided by using:

- **Working Set Model**: In this model we try to use locality of pages. Locality of pages refers to the set of pages actively used together. During execution processes move from locality to locality. Working set model accesses the recently used pages from memory and tries to approximate the locality. After finding that out we can appropriately allocate frames to each process.

- **Frame Frequency**: In this model we make sure each process has a minimum number of page faults and maximum number of page faults. If a process doesn't have enough page faults that means it has too many frames and if a process has too many page faults that means it has very few pages.

  - If page fault rate > upper limit = allocate frames
  - If page fault rate < lower limit = remove frames
