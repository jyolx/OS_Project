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
    - [Folders and their purpose](#folders-and-their-purpose)

    - [Functionalities](#functionalities)
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

<img src="images/shm.png" alt="SHM system call" width="400">

### ii. Rename File
- `rename()`

  - renames the given file to the name you want to change it to
  - rename(oldname,newname)

Wrapper function for renaming file
```bash
$ myrename
```

<img src="images/rename().png" alt="Rename system call" width="400">

### iii. Raise signal
- `raise()`

  - implemented `raise(SIGKILL)` which terminates the current process where it has been raised

Wrapper function for raising signal
```bash
$ mysignal
```

<img src="images/raise().png" alt="Raise system call" width="300">

### iv. Get process information
- `getprocinfo()`

    - gives information about the current process pid, its parent pid and state of the process

Wrapper function for getting information about the current process
```bash
$ myproc
```

<img src="images/getprocinfo().png" alt="Procinfo system call" width="400">

## 4. Developing a multiple client web server

We developed a multi-threaded web server capable of handling up to 10 client requests concurrently, leveraging a thread pool architecture to manage the load. The server utilizes a producer-consumer model, where client requests act as the producers and the threads in the pool serve as consumers. The server supports essential HTTP methods to handle a wide variety of client interactions. Each request is logged, capturing relevant details of the server's activity and storing them in a log file for monitoring and debugging purposes. Authentication mechanisms are integrated for restricted resources, ensuring secure access control. Additionally, the server is equipped with robust error handling capabilities, managing various HTTP errors and providing appropriate responses to clients, contributing to a reliable and secure web service.

## Folders and their purpose

### i. Client
- Contains sample client python code in `client.py` to simulate client requests.

- Run `clean.sh` to clear the previously stored data received from the server.
- A `data` directory gets created to store the data requested by the client.

### ii. Server

- ### config
  - `server.config` : defines the paramaters of port, network, max thread and document root of the server

  - `users.txt` : keeps track of all users and with their passwords 

- ### data
  - Contains server data requested by the client.

  - Data can be an image, text, audio or html files.
  - Some data are under the `secure` folder which means the client needs to be authenticated to access them.

- ### include
  - Contains the header files to be included to carry out the necessary functionalities

    - `authentication.h`

    - `config.h`
    - `http.h`
    - `logger.h`
    - `server.h`

- ### logs
  - Stores all the logs throughout the server in a `server.log` file

- ### src
  - Contains the files for carrying out all the functionalities

    - `authentication.c` : Decryptes the base64 encoded username and password of the client and checks for a match in `config/users.txt`.

    - `config.c` : Loads the `server.conf` onto a data structure.

    - `http.c` : 
      - Contains functions which handle the HTTP request methods like `GET`, `POST`, `PUT` and `DELETE`.

      - It also checks if the client is authorised if it requests data in the `data/secure` folder. 
      - It also sends back the appropriate HTTP response.

    - `logger.c` : A basic implememtation of a logger to log the server actions on to `logs/server.log`.

    - `server.c` : Using TCP it offers a connection with the clients, implements multithreading to handle multiple client requests and uses semaphores for synchronisation.

    - `main.c` : Root of the server.


## Functionalities (with `curl` commands)

- ### Serves Static Files

  - It serves static  content like HTML, images, and audio from the `server/data` folder to the clients, ensuring content is sent in response to HTTP requests.

- ### Supports different HTTP Methods
  - `GET` : used by clients to retrieve data from the server.
    ```bash 
    $ curl http://localhost:8080/index.html
    ```
    <img src="images/get.png" alt="get" width="500">
    <img src="images/get_log.png" alt="get log" width="600">

    <img src="images/get_output.png" alt="get output" width="300">

  - `POST` : used by clients to send data to the server.
    ```bash 
    $ curl -X POST http://localhost:8080/application.json -H "Content-Type: application/json" -d '{"name": "Jane Smith", "email": "jane.smith@example.com"}'
      ```
    <img src="images/post.png" alt="400" width="500">
    <img src="images/post_log.png" alt="400 log" width="600">

  - `PUT` : used to update an existing resource or create a new resource if it does not already exist.
    ```bash 
    $ curl -X PUT http://localhost:8080/test.txt -H "Content-Type: text/plain" -T data/test.txt
    ```
    <img src="images/put.png" alt="400" width="500">
    <img src="images/put_log.png" alt="400 log" width="600">

  - `DELETE` : used to remove a resource from the server.
    ```bash 
    $ curl -X DELETE http://localhost:8080/pic1.jpg
    ```
    <img src="images/delete.png" alt="400" width="500">
    <img src="images/delete_log.png" alt="400 log" width="600">

- ### Logging Requests
  - The server logs each client request and its response in the `server/logs/server.log` file, helping monitor activity and troubleshoot issues.

    <img src="images/initialized_server.png" alt="400" width="600">

- ### Handles Multiple Content Types
  - The server can serve various types of content (e.g., images, text, HTML, audio) based on the request type. The content is served from the `server/data` folder.

- ### Error Handling
  - The server includes error handling mechanisms and returns appropriate HTTP error responses which include : 

    - `400 : Bad Request`
      ```bash 
      $ curl -X HEAD http://localhost:8080/hi.txt
      ```
      <img src="images/400.png" alt="400" width="500">
      <img src="images/400_log.png" alt="400 log" width="600">

    - `401 : Unauthorized`
      ```bash 
      $ curl -u username:wrongpassword http://localhost:8080/secure/audio1.mp3
      ```
      <img src="images/401.png" alt="401" width="500">
      <img src="images/401_log.png" alt="401 log" width="600">

    - `404: Not Found`
      ```bash 
      $ curl -u http://localhost:8080/hi.txt
      ```
      <img src="images/404.png" alt="404" width="500">
      <img src="images/404_log.png" alt="404 log" width="600">

    - `500 : Internal Server Error`
      ```bash 
      $ curl -u username:password http://localhost:8080/secure/pic3.jpeg
      ```
      <img src="images/500.png" alt="500" width="500">
      <img src="images/500_log.png" alt="500 log" width="600">

  - On the other hand, it returns `200 : OK` which tells us that the client request has been handled successfully.

- ### Thread Pooling
  - The server uses a thread pool of 10 threads to handle multiple client requests concurrently, improving performance and scalability by reusing threads for handling subsequent client connections.

  - Client requests are `enqueue()` to the queue data structure and worker threads `dequeue()` the requests from the queue.
  - Access to the queue is synchronised using semaphores as used in the `Producer-Consumer problem`.
  - The client requests act like the Producers and the threads which perform these requests act like the Consumers.

- ### Configuration File
  - The server is configured through a `server/config/server.config` file, which defines the parameters like server port, network settings, and maximum threads.

- ### Basic Authentication
  - The server requires authentication for certain resources stored in the `server/data/secure` folder. Clients must provide valid credentials (defined in `server/config/users.txt`) before being granted access to these resources.

    ```bash
    $ curl -u username:password http://localhost:8080/secure/audio1.mp3
    ```
    <img src="images/auth.png" alt="Authentication" width="500">
    <img src="images/auth_log.png" alt="Authentication log" width="600">

- ### Server Shutdown
  - Signal handling is used for the graceful shutdown of the server. After raising `SIGINT` with `Ctrl-C`, the server deallocates the queue and closes the socket file descriptor.

    <img src="images/server_shutdown.png" alt="Server shutdown" width="600">

## 5. Contributors
<a href="https://github.com/jyolx/OS_Project/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=jyolx/OS_Project" />
</a>
