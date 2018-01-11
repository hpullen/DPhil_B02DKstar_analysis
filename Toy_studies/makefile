CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so

INC = -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc

OUT = PlotPulls
SIG = PlotSignificancePulls

.PHONY: all out sig

all: out sig

out:
	$(CC) src/plotPulls.cpp -o $(OUT) $(INC) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PLOTSTYLELIB)

sig:
	$(CC) src/plotPulls_significance.cpp -o $(SIG) $(INC) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PLOTSTYLELIB)


.PHONY: clean
clean:
	rm -f $(OUT) $(SIG)
