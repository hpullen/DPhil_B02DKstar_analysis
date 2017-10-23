CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

.PHONY: all training

all: training

training:
	$(CC) src/makeTrainingTuple.cpp -o MakeTrainingTuple $(TRAINING_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

.PHONY: clean
clean:
	rm -f $(TRAINING_OUT)

