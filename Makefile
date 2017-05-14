#ctatli 640427

CC = gcc
CFLAGS = -Wall -std=gnu99

EXE = simpleminer
SRC = server.c log.c
OBJ = server.o log.o

simpleminer: $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

clean:
	rm *.o
