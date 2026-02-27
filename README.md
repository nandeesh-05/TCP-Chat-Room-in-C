# TCP Chat Room in C

## 📌 Description

Implemented a real-time TCP-based Chat Room application in C where multiple clients connect to a central server and communicate with individual users or groups. The server manages message routing, authentication, and concurrent client handling using multithreading.

## 🎯 Key Features

* User registration and login with credential validation
* Thread-per-client architecture for concurrent communication
* One-to-one (single) chat support
* Group chat using message flags
* Online user tracking with join/leave notifications
* Graceful shutdown using SIGINT with broadcast offline alerts

## 🛠️ Tech Stack

* Programming Language: C
* Platform: Linux
* Networking: TCP Sockets
* Concurrency: POSIX Threads (pthreads)
* Concepts: File Handling, Signals

## 🔹 Compilation & Execution

This project was compiled directly using GCC.

### Server

```
gcc server.c functions.c -o server -lpthread
./server
```

### Client

```
gcc client.c -o client -lpthread
./client
```

## ⚙️ Architecture Overview

* Server handles multiple clients using a thread-per-client model.
* Shared data structures store online users and connection details.
* Mutex locks are used to ensure synchronization during concurrent access.
* Messages are routed by the server based on message type (single or group).

## 📚 What I Learned

* Designing concurrent server architecture using threads
* Implementing synchronization with mutex for shared resources
* Reliable client–server communication using TCP
* Handling signals for graceful shutdown
* Managing real-time messaging systems

## 🧠 Skills Demonstrated

* C Programming
* TCP Socket Programming
* Multithreading with POSIX Threads
* Synchronization and Mutex Handling
* Client–Server Architecture
* Debugging and Problem Solving

