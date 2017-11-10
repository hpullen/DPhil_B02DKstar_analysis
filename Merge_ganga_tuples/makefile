CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

OUT = MakeTuple

.PHONY: all tuple

all: tuple

tuple:
	$(CC) src/makeTuple.cpp -o $(OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

.PHONY: clean
clean:
	rm -f $(OUT)
