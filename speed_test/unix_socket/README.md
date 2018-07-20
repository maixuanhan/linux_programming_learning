# UNIX Domain Socket example

SOCK_STREAM unix domain socket example

## Introduction

### receiver

- Create a socket.
- Remove any existing file with the same pathname as that to which we want to bind the socket.
- Construct an address structure for the server's socket, bind the socket to that address, and mark the socket as a listening socket.
- Accept a connection, obtaining a new socket, cfd, for the connection.
- Read all of the data from the connected socket and calculate the sum.
- Close the connected socket cfd.
- Close the server socket sfd.

### sender

- Create a socket.
- Construct the address structure for the server's socket and connect to the socket at that address.
- Write some data.
- Close the socket sfd. The server will see EOF.

## Compilation and Running

Issue `make` command in the directory, you'll get 2 binary files, `sender` and `receiver`.

Run `receiver` to create a unix domain socket server.

Run `sender` to start sending data via socket.
