CC = g++
SRC = main.cpp bst.cpp hand_over_hand.cpp reader_writer.cpp
OP = bst
CFLAGS += -pthread -g
OBJ := $(SRC:.c=.o)

all: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(OP)

clean:
	rm -f *.o *.txt $(OP)