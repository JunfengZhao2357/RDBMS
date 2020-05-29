CXX=g++-8
ifeq ($(shell uname), Darwin) 
	CXX=g++
endif
CXXFLAGS=-g -std=c++17 -Wall -pedantic
BIN=autograder

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)