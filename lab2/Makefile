CC = g++
SRC1 = main.cpp mergesort.cpp bucketsort.cpp locks.cpp
OP1 = mysort
OP2 = counter
CFLAGS = -pthread -O3
OBJ1 := $(SRC1:.c=.o)

SRC2 = counter.cpp locks.cpp
OBJ2 := $(SRC2:.c=.o)

all: $(OBJ1) $(OBJ2)
	$(CC) $(OBJ1) $(CFLAGS) -o $(OP1)
	$(CC) $(OBJ2) $(CFLAGS) -o $(OP2)

clean:
	rm -f *.o *.txt $(OP)
