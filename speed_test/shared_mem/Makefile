# Compile
CC = gcc

CFLAGS = -Wall

LIB = -lrt -lpthread

BIN1 = sender
BIN2 = receiver

SRCS1 = *.h sender.c
SRCS2 = *.h receiver.c

all:
	$(CC) $(CFLAGS) -o $(BIN1) $(SRCS1) $(LIB)
	$(CC) $(CFLAGS) -o $(BIN2) $(SRCS2) $(LIB)

clean:
	rm -f $(BIN1) $(BIN2)
