CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` 

SELECTION_OUT = SelectionEfficiency
WEIGHT_OUT = WeightEfficiencies
PID_OUT = PIDefficiency
SELECTION_LOW_OUT = LowMassSelectionEfficiency

CUTINC = -I/home/pullen/analysis/B02DKstar/Libraries/CutReader/inc
CUTLIB = /home/pullen/analysis/B02DKstar/Libraries/CutReader/libCutReader.so

.PHONY: all sel weight pid low_sel

all: sel weight pid low_sel

sel:
	$(CC) src/selectionEfficiency.cpp -o $(SELECTION_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(CUTINC) $(CUTLIB)

weight:
	$(CC) src/weightEfficiencies.cpp -o $(WEIGHT_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

pid:
	$(CC) src/PIDefficiency.cpp -o $(PID_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

low_sel:
	$(CC) src/selectionEfficiency_lowMass.cpp -o $(SELECTION_LOW_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

.PHONY: clean
clean:
	rm -f $(SELECTION_OUT) $(WEIGHT_OUT) $(PID_OUT) $(SELECTION_LOW_OUT)
