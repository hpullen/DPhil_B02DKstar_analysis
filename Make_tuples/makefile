CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

TRAINING_OUT = MakeTrainingTuple
SELECTED_OUT = MakeSelectedTuple

.PHONY: all training selected

all: training selected

training:
	$(CC) src/makeTrainingTuple.cpp -o $(TRAINING_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

selected:
	$(CC) src/makeSelectedTuple.cpp -o $(SELECTED_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

.PHONY: clean
clean:
	rm -f $(TRAINING_OUT) $(SELECTED_OUT)
