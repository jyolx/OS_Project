# Operating Systems Course Project
## 1. Implementing xv6 System Calls
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
