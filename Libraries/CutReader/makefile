CC = g++
CFLAGS = -Wall -Wpedantic -g -std=c++11
ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

all : libCutReader.so TestReader

obj/CutReader.o : src/CutReader.cpp
	$(CC) -Iinc $(CFLAGS) $(ROOTINC) -fPIC -c -o $@ $<

libCutReader.so : obj/CutReader.o
	$(CC) -shared -o $@ $<

obj/main.o : src/main.cpp
	$(CC) -Iinc $(CFLAGS) $(ROOTINC) -c -o $@ $<

TestReader : obj/main.o libCutReader.so
	$(CC) libCutReader.so $(ROOTLIB) -o $@ obj/main.o

