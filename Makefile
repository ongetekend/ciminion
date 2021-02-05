UNAME := $(shell uname)

.POSIX:
.SUFFIXES:
CXX = g++
CXXFLAGS = -I./include -Wall -g -O2 -std=c++11 -pthread -march=native
LDFLAGS = 
LDLIBS = -lntl -lgmp -lm -lpthread

.PHONY: all
all: main 

main: src/main.o 
	$(CXX) $(LDFLAGS) -o src/$@ $^ $(LDLIBS)

.PHONY: clean
clean:
	rm -rf bin src/main src/main.o

.PHONY: install
install: main
	mkdir -p bin
	cp src/main bin

.SUFFIXES: .cpp .o
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

