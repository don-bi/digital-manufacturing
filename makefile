CC = g++
CXX = g++ -std=c++20

CFLAGS = -g -Wall
CXXFLAGS = -g -Wall

square: square.o

.PHONY: clean
clean:
	rm -f *.o square

.PHONY: all
all: clean square
