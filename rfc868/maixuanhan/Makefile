# Compile
CC = gcc

CFLAGS = -Wall 

LIB = -lrt -lpthread

BIN1 = client
BIN2 = server

SRCS0 = common.h
SRCS1 = client.c
SRCS2 = server.c

all:
	$(CC) $(CFLAGS) -o $(BIN1) $(SRCS0) $(SRCS1) $(LIB)
	$(CC) $(CFLAGS) -o $(BIN2) $(SRCS0) $(SRCS2) $(LIB)

clean:
	rm -f $(BIN1) $(BIN2)
