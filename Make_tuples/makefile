CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

CUTINC = -I/home/pullen/analysis/B02DKstar/Libraries/CutReader/inc
CUTLIB = /home/pullen/analysis/B02DKstar/Libraries/CutReader/libCutReader.so

TRAINING_OUT = MakeTrainingTuple
VARS_OUT = MakeVarsTuple
SELECTED_OUT = MakeSelectedTuple
MC_SELECTED_OUT = MakeSelectedMCtuple

.PHONY: all training vars selected mc

all: training vars selected mc

training:
	$(CC) src/makeTrainingTuple.cpp -o $(TRAINING_OUT) -g $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(CUTINC) $(CUTLIB)

vars:
	$(CC) src/makeTupleWithVars.cpp -o $(VARS_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

selected:
	$(CC) src/makeSelectedTuple.cpp -o $(SELECTED_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

mc:
	$(CC) src/makeSelectedMCtuple.cpp -o $(MC_SELECTED_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

.PHONY: clean
clean:
	rm -f $(TRAINING_OUT) $(VARS_OUT) $(SELECTED_OUT) $(MC_SELECTED_OUT)
