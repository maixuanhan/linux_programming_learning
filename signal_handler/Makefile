# Compile
CC = gcc

# -m32 to force gcc build 32-bit
CFLAGS = -Wall -m32 

BIN = signal_handler

SRCS = *.c

all:
	$(CC) $(CFLAGS) -o $(BIN) $(SRCS)

clean:
	rm -f $(BIN)
