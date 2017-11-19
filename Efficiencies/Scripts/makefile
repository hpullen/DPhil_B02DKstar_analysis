CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` 

SELECTION_OUT = SelectionEfficiency
WEIGHT_OUT = WeightEfficiencies
PID_OUT = PIDefficiency

.PHONY: all sel weight pid

all: sel weight pid

sel:
	$(CC) src/selectionEfficiency.cpp -o $(SELECTION_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

weight:
	$(CC) src/weightEfficiencies.cpp -o $(WEIGHT_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

pid:
	$(CC) src/PIDefficiency.cpp -o $(PID_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

.PHONY: clean
clean:
	rm -f $(SELECTION_OUT) $(WEIGHT_OUT) $(PID_OUT)
