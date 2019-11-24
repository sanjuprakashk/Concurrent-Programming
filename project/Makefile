CC = gcc
SRC = bst.c
OP = bst
CFLAGS = -pthread -g
OBJ := $(SRC:.c=.o)

all: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(OP)

clean:
	rm -f *.o *.txt $(OP)