CC = g++
SRC = main.cpp mergesort.cpp bucketsort.cpp
OP = mysort
CFLAGS = -pthread -O3
OBJ := $(SRC:.c=.o)

all: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(OP)

clean:
	rm -f *.o *.txt $(OP)
