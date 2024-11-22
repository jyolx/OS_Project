# Operating Systems Course Project

## Table of Contents
1. [Introduction](#1-introduction)
2. [Getting Started](#2-getting-started)
3. [Implementing xv6 System Calls](#3-implementing-xv6-system-calls)
   - [Shared Memory](#i-shared-memory)
   - [Rename File](#ii-rename-file)
   - [Raise Signal](#iii-raise-signal)
   - [Get Process Information](#iv-get-process-information)
4. [Developing a Multiple Client Web Server](#4-developing-a-multiple-client-web-server)
5. [Contributors](#5-contributors)

## 1. Introduction

Operating systems form the backbone of modern computing systems, enabling seamless interaction between hardware and software. This project explores two key aspects of operating systems: customizing the lightweight educational operating system xv6 by implementing system calls, and building a robust multi-threaded web server capable of handling multiple clients concurrently.

### Goals:
1. **Extend xv6 functionality** : By implementing and demonstrating custom system calls, we deepen our understanding of kernel development and inter-process communication.
2. **Develop a feature-rich web server** : The web server showcases practical knowledge of multi-threading, synchornisation, HTTP protocol handling, and server management.

## 2. Getting Started

Prerequisites
- Unix-based operating system (for running xv6 and the server)
- ` gcc ` , ` make ` and ` qemu ` (for compilation)
- ` git ` (to clone the repository)

Clone the the repository
```bash
$ git clone https://github.com/jyolx/OS_Project
```

## 3. Implementing xv6 System Calls

We enhanced the xv6-riscv operating system by adding new system calls, each tailored to address specific use cases. Our modifications involved editing both kernel and user-space code. 

To implement our system calls, we wrote their core functionalities in the kernel space, specifically in `sysproc.c` and `sysfile.c`. Each system call was assigned a unique integer identifier in `syscall.h`. This identifier, along with the function definition, was mapped in the system call table within `syscall.c`, enabling the kernel to recognize and route the calls. To make these system calls accessible in user space, we defined their prototypes in `user.h`. The `usys.pl` script was then used to generate user-space stubs that act as entry points for the system calls. Finally, we created user programs to test our system calls and added their object files to the `Makefile`, ensuring they were compiled and linked during the build process.

Each system call has its wrapper function in the user space which you can run once the xv6 system is booted.

To run the xv6 system calls,

Navigate to the xv6-riscv directory and run the following commands
```bash 
$ cd Project1_xv6CustomizeSystemCalls/xv6-riscv
$ make clean
$ make qemu 
```
To exit enter  ```Ctrl+A X```

### You can find the following system calls:

### i. Shared Memory
- `shmget()`

  - creates the shared memory and returns the shared memory ID (shmid) of the shared memory created
- `shmat()`
  - attaches the shared memory space created with the address space of the process 
- `shmdt()`
  - detaches the shared memory
- `shmctl()`
  - destroys the shared memory

Wrapper function for writing into shared memory is
```bash
$ mywritershm
```
Once you enter a string to write into the shared memory run the wrapper function for reading into shared memory
```bash
$ myreadershm
```

![SHM](images/shm.png "SHM system call")

### ii. Rename File
- `rename()`

  - renames the given file to the name you want to change it to
  - rename(oldname,newname)

Wrapper function for renaming file
```bash
$ myrename
```

![Rename](images/rename().png "Rename system call")

### iii. Raise signal
- `raise()`

  - implemented `raise(SIGKILL)` which terminates the current process where it has been raised

Wrapper function for raising signal
```bash
$ mysignal
```
![Raise](images/raise().png "Raise system call")

### iv. Get process information
- `getprocinfo()`

    - gives information about the current process pid, its parent pid and state of the process

Wrapper function for getting information about the current process
```bash
$ myproc
```
![Procinfo](images/getprocinfo().png "Procinfo system call")

## 4. Developing a multiple client web server

We built a multi-threaded web server capable of handling multiple clients concurrently. This server supports essential HTTP methods and includes advanced features.

- Serves Static Files
- Supports different HTTP Methods like `GET`, `POST`, `PUT` and `DELETE`
- Logging Requests
- Handles Multiple Content Types
- Error Handling
- Thread Pooling
- Configuration File
- Basic Authentication

## 5. Contributors
<a href="https://github.com/jyolx/OS_Project/contributors">
  <img src="https://contrib.rocks/image?repo=jyolx/OS_Project/contributors" />
</a>