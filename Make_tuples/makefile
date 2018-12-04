CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 -g3

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

CUTINC = -I/home/pullen/analysis/B02DKstar/Libraries/CutReader/inc
CUTLIB = /home/pullen/analysis/B02DKstar/Libraries/CutReader/libCutReader.so

PARINC = -I/home/pullen/analysis/B02DKstar/Libraries/ParameterReader/inc
PARLIB = /home/pullen/analysis/B02DKstar/Libraries/ParameterReader/libParameterReader.so
INC = -I/home/pullen/analysis/B02DKstar/Libraries/pdfs/inc
PDFLIB = /home/pullen/analysis/B02DKstar/Libraries/pdfs/libPdfs.so

TRAINING_OUT = MakeTrainingTuple
VARS_OUT = MakeVarsTuple
SELECTED_OUT = MakeSelectedTuple
MC_SELECTED_OUT = MakeSelectedMCtuple
SPLIT_OUT = SplitTuple
WEIGHT_OUT = WeightTuple
L0_OUT = AddL0Weights

.PHONY: all training vars selected mc split weight L0

all: training vars selected mc split weight L0

training:
	$(CC) src/makeTrainingTuple.cpp -o $(TRAINING_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(CUTINC) $(CUTLIB)

vars:
	$(CC) src/makeTupleWithVars.cpp -o $(VARS_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

selected:
	$(CC) src/makeSelectedTuple.cpp -o $(SELECTED_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(CUTINC) $(CUTLIB)

mc:
	$(CC) src/makeSelectedMCtuple.cpp -o $(MC_SELECTED_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(CUTINC) $(CUTLIB)

split:
	$(CC) src/splitTuple.cpp -o $(SPLIT_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

weight:
	$(CC) src/weightTuple.cpp -o $(WEIGHT_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PARINC) $(PARLIB) -lRooFit -lRooStats $(INC) $(PDFLIB)

L0:
	$(CC) src/addL0weights.cpp -o $(L0_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PARINC) $(PARLIB) -lRooFit -lRooStats $(INC) $(PDFLIB)

.PHONY: clean
clean:
	rm -f $(TRAINING_OUT) $(VARS_OUT) $(SELECTED_OUT) $(MC_SELECTED_OUT)
