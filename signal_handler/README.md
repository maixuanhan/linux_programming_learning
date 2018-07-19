# Use non-signal-atomic type may result in race condition

## Introduction

This example give a simple scenario where the race condition happens.

- There is a global variable of `unsigned long long` type. This size is double of `int` size in 32-bit system.
- There is a periodic timer that updates the value of the global variable to `0x1010101010101010` for every expiry time.
- Continuosly update the value of the global variable to `0xffffffffffffffff` in an infinite loop.
- The expected result is that the value of the global variable is one of 2 above values. But the actual result is that sometimes the value of the variable is messed up.

## Compilation and Running

Issue `make` command in the directory, you'll get the binary file named `signal_handler`.

You may encounter the problem that

```
fatal error: gnu/stubs-32.h: No such file or directory
```

It's because the glibc-devel 32 bit is missing, try to install it and the problem will be gone.

After successful compilation, run the command and see the output.

```
    ./signal_handler
```

**Note**: If the program is exited automatically without any problem, it means that the race condition has just happened.
