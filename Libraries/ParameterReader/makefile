CC = g++
CFLAGS = -Wall -Wpedantic -g -std=c++11
ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

all : libParameterReader.so 

obj/ParameterReader.o : src/ParameterReader.cpp
	$(CC) -Iinc $(CFLAGS) $(ROOTINC) -fPIC -c -o $@ $<

libParameterReader.so : obj/ParameterReader.o
	$(CC) -shared -o $@ $<


