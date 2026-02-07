CC = g++
CXX = g++ -std=c++20

CFLAGS = -g -Wall
CXXFLAGS = -g -Wall

container: container.o

.PHONY: clean
clean:
	rm -f *.o container

.PHONY: all
all: clean container
