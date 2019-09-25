CC = gcc
SRC = main.c quicksort.c
OP = mysort

OBJ := $(SRC:.c=.o)

all: $(OBJ)
	$(CC) $(OBJ) -o $(OP)

clean:
	rm -f *.o *.txt
