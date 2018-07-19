# POSIX shared memory example

List all shared memory objects in the system
```
    ls -l /dev/shm
```

## Introduction

There are 2 programs which use a same shared memory object named `test_speed_shared_mem`.

### sender
- Create a shared memory object.
- Set size of the shared memory object.
- Map the shared memory object to process's memory.
- Write some data.
- Send `SIGUSR1` to the receiver.

### receiver
- Wait for `SIGUSR1` signal from the sender.
- Open the shared memory object.
- Get the information of the shared memory object including object size.
- Map the shared memory object to process's memory as read-only.
- Read the data, do some calculation.
- Unlink the shared memory object.

## Compilation and Running

Issue `make` command in the directory, you'll get 2 binary files, `sender` and `receiver`.

Run `receiver` to get its PID.

Run `sender` with an argument is the PID of the receiver. For example

```
    ./sender 12906
```
