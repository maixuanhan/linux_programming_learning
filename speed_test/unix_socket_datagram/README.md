# UNIX Domain Socket example

SOCK_DGRAM unix domain socket example

## Introduction

### receiver

- Create a socket.
- Remove any existing file with the same pathname as that to which we want to bind the socket.
- Construct an address structure for the server's socket, bind the socket to that address.
- Read all of the data from the connected socket and calculate the sum.
- Close the server socket sfd.

### sender

- Create a socket.
- Construct an address structure for the socket.
- Send some data to the address.
- Close the socket sfd.

## Compilation and Running

Issue `make` command in the directory, you'll get 2 binary files, `sender` and `receiver`.

Run `receiver` to create a unix domain socket server.

Run `sender` to start sending data via socket.
