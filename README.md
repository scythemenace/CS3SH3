# Operating Systems (COMP SCI 3SH3) – Fall 2024

This repository contains my complete coursework for the Operating Systems course (COMPSCI 3SH3) during Fall 2024 at McMaster University. The course covered both theoretical concepts and practical implementation of OS components in C.

---

## 📚 Course Topics Covered

- **Processes and Threads**
- **Scheduling Algorithms**
- **Memory Management (Paging, TLB, Page Tables)**
- **File Systems and Disk Scheduling**
- **Synchronization and Communication (Semaphores, Mutexes, Threads)**
- **Protection and Security**
- **OS Design and Structure**

---

## 🧠 Assignments

### `Assignment_1` – Kernel Module & /proc Filesystem
- Built a Linux kernel module that creates a `/proc/seconds` entry showing uptime since module load.
- Gained experience working with jiffies, the `/proc` pseudo-filesystem, and kernel-space programming.

### `Assignment_2` – Custom Shell with History
- Created a minimal Unix shell (`osh>`) that supports background execution (`&`), and a 5-command rolling history buffer with `!!` support.
- Practiced process creation using `fork()` and `exec()`, and command handling in a custom CLI interface.

### `Assignment_3` – The Sleeping TA Problem
- Implemented a classic synchronization problem using POSIX threads, mutexes, and semaphores.
- Simulated a TA helping students, with a finite number of chairs (buffered waiting room) and sleeping/waking logic.

### `Assignment_4` – Virtual Memory and TLB (Personal Favorite)
- Simulated a full **memory management unit (MMU)**:
  - Address translation from logical to physical using page tables and a 16-entry TLB.
  - Implemented FIFO-based page replacement and memory-mapped access using `mmap()`.
- This was the **most challenging and rewarding assignment**; I implemented my own solution in the `solution2/` folder to fully grasp TLB management and page fault handling.

### `Assignment_5` – Disk Scheduling Algorithms
- Implemented and compared six disk scheduling algorithms: `FCFS`, `SSTF`, `SCAN`, `C-SCAN`, `LOOK`, `C-LOOK`.
- Parsed cylinder requests from a binary file and tracked total head movement for each policy.

---

## Labs

These labs provided hands-on exposure to core OS concepts like kernel modules, process creation, memory management, threading, and file systems, forming the foundation for the assignments.

### `Lab 1` – Linux Kernel Modules & Process Representation
- Created and managed a **custom kernel module**, observing lifecycle events via `dmesg`.
- Accessed and printed fields from the `task_struct` of the `init_task` (PID 0).
- Learned to compile modules using `make`, and insert/remove them using `insmod` and `rmmod`.
- Printed kernel constants like `HZ`, `jiffies`, and `GOLDEN_RATIO_PRIME`, and invoked internal kernel functions like `gcd()`.

### `Lab 2` – Process Creation and POSIX Threads
- Built a **process tree of 9 processes** using `fork()` and `wait()`, with strict PID printing rules.
- Implemented **multithreaded summation** over two sublists using `pthread_create`, `pthread_join`, and heap-allocated parameter structs.
- Learned process/thread lifecycle management and memory sharing strategies.

### `Lab 3` – Synchronization with Mutexes and Semaphores
- Simulated a **shared bank account** accessed by deposit and withdraw threads.
- Part I used **mutex locks** to avoid race conditions.
- Part II added **semaphores** to enforce balance bounds (e.g., no overdraft or over-deposit beyond $400).
- Practiced using `pthread_mutex_t`, `sem_t`, and thread-safe printing/debugging.

### `Lab 4` – Address Translation & Memory Mapped Files
- Simulated **virtual to physical address translation** using a custom page table with bitwise logic.
- Parsed logical addresses, split them into page number and offset, then used C macros and shifts to compute physical addresses.
- Used `mmap()` to memory map a binary file (`numbers.bin`), and accessed data via `memcpy()` — foundational for Assignment 4.

### `Lab 5` – Inodes and File Protection
- Retrieved file metadata (inode, size, permissions, timestamps) using both CLI tools (`ls -li`, `stat`) and a C program leveraging the `stat` struct.
- Parsed file permission bits using bitwise flags like `S_IRUSR`, `S_ISDIR`.
- Logged terminal session activity using `script` and validated protection changes using `chmod`.

---

## Notes

The `notes/` folder contains my personal Markdown notes covering challenging concepts like:

- Paging and Virtual Memory
- TLB vs Page Tables
- Disk Scheduling trade-offs
- Thread Synchronization patterns

These helped reinforce both lecture content and lab work.

---

## Tutorial Questions

Contains solutions to OS tutorial problems discussed throughout the term, useful for quick review and exam prep.

---

## 📖 Reference

- Textbook: *Operating System Concepts* (Silberschatz et al.) – also included in PDF form for reference.

---

## 📂 Structure

```bash
.
├── Assignment_1/
├── Assignment_2/
├── Assignment_3/
├── Assignment_4/
│   └── solution2/     # My individual implementation
├── Assignment_5/
├── Lab_1/ through Lab_5/
├── Tutorial Questions/
├── notes/
├── Operating System Concepts.pdf
└── .gitignore
