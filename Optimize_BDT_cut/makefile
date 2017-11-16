CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats

PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
INC = -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc

OUT = OptimizeBDTcut

.PHONY: all twobody

all: twobody

twobody:
	$(CC) src/optimizeBDTcut.cpp -o $(OUT) $(INC) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PLOTSTYLELIB)


.PHONY: clean
clean:
	rm -f $(OUT)
