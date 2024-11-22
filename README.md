# Operating Systems Course Project

## 1. Implementing xv6 System Calls

To run the xv6 system calls,

Clone the the repository
```bash
git clone https://github.com/jyolx/OS_Project
```

Navigate to the xv6-riscv directory and run the following commands
```bash 
cd Project1_xv6CustomizeSystemCalls/xv6-riscv
make clean
make qemu 
```

Each system call has its wrapper function in the user space which you can run once the xv6 system is booted.

### You can find the following system calls

- shm functions -> `shmget()`,`shmat()`,`shmdt()`,`shmctl()`
- file function -> `rename()`
- signal -> `raise()`
  - implemented `raise(SIGKILL)` which terminates the current process where it has been raised
- process function -> `getprocinfo()`
  
    - gives information about the current process pid, parent pid and state

## 2. Developing a multiple client web sever 
- Serves Static Files
- Supports different HTTP Methods like `GET`, `POST`, `PUT` and `DELETE`
- Logging Requests
- Handles Multiple Content Types
- Error Handling
- Thread Pooling
- Configuration File
- Basic Authentication