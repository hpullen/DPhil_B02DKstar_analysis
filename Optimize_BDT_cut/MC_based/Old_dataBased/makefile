CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats

PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
INC = -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc

OUT = OptimizeBDTcut
ONE_D_OUT = OptimizeBDTcut_1D

.PHONY: all twobody oneD

all: twobody oneD

twobody:
	$(CC) src/optimizeBDTcut.cpp -o $(OUT) $(INC) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PLOTSTYLELIB)

oneD:
	$(CC) src/optimizeBDTcut_1D.cpp -o $(ONE_D_OUT) $(INC) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PLOTSTYLELIB)


.PHONY: clean
clean:
	rm -f $(OUT)
