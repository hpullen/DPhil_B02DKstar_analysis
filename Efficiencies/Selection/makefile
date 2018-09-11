CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` 

SELECTION_OUT = SelectionEfficiency
SELECTION_LOW_OUT = LowMassSelectionEfficiency
SELECTION_RHO_OUT = RhoSelectionEfficiency
SELECTION_LAMBDA_OUT = LambdaSelectionEfficiency

CUTINC = -I/home/pullen/analysis/B02DKstar/Libraries/CutReader/inc
CUTLIB = /home/pullen/analysis/B02DKstar/Libraries/CutReader/libCutReader.so

.PHONY: all sel weight pid low_sel rho_sel lambda_sel

all: sel weight pid low_sel rho_sel lambda_sel

sel:
	$(CC) src/selectionEfficiency.cpp -o $(SELECTION_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(CUTINC) $(CUTLIB)

low_sel:
	$(CC) src/selectionEfficiency_lowMass.cpp -o $(SELECTION_LOW_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

rho_sel:
	$(CC) src/selectionEfficiency_rho.cpp -o $(SELECTION_RHO_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

lambda_sel:
	$(CC) src/selectionEfficiency_lambda.cpp -o $(SELECTION_LAMBDA_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB)

.PHONY: clean
clean:
	rm -f $(SELECTION_OUT) $(WEIGHT_OUT) $(PID_OUT) $(SELECTION_LOW_OUT) $(SELECTION_RHO_OUT) $(SELECTION_LAMBDA_OUT)
