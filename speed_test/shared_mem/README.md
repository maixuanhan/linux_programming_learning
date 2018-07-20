# POSIX shared memory example

This example demonstrates the way to send and receive data between 2 processes using shared memory synchronized by a binary semaphore.

List all shared memory objects, semaphore objects in the system
```bash
    ls -l /dev/shm
```

## Introduction

There are 2 programs which use a same shared memory object.

### receiver
- Create a shared memory object.
- Set the shared memory size.
- Map the shared memory object to process's memory. The last 4 bytes of the memory block is used for sending signal between 2 processes.
    - 1: Ready for reading, writing is forbidden.
    - 0: Ready for writing, reading is forbidden.
- Create a named semaphore to protect that 4-byte block.
- Wait the signal from the sender and read data.
- Unlink the shared memory object.
- Unlink the semaphore object.

### sender
- Open the shared memory object.
- Get the shared memory size.
- Map the shared memory object to process's memory.
- Write some data.
- Give a signal to the receiver.

## Compilation and Running

Issue `make` command in the directory, you'll get 2 binary files, `sender` and `receiver`.

Run `receiver` to create appropriate shared memory object and semaphore.

Run `sender` to start sending data via shared memory.
