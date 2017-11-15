CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

TRAINING_OUT = MakeTrainingTuple
VARS_OUT = MakeVarsTuple
SELECTED_OUT = MakeSelectedTuple

.PHONY: all training vars selected

all: training vars selected

training:
	$(CC) src/makeTrainingTuple.cpp -o $(TRAINING_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

vars:
	$(CC) src/makeTupleWithVars.cpp -o $(VARS_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

selected:
	$(CC) src/makeSelectedTuple.cpp -o $(SELECTED_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

.PHONY: clean
clean:
	rm -f $(TRAINING_OUT) $(VARS_OUT) $(SELECTED_OUT)
