#ctatli 640427

CC = gcc
CFLAGS = -Wall -std=gnu99

EXE = server
SRC = server.c log.c sstp.c hash.c sha256.c util.c
OBJ = server.o log.o sstp.o hash.o sha256.o util.o

server: $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

clean:
	rm *.o
