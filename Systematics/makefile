CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 -g3

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so

INC = -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc

OUT = PlotSystematics

.PHONY: all out 

all: out 

out:
	$(CC) src/plotSystematics.cpp -o $(OUT) $(INC) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PLOTSTYLELIB)


.PHONY: clean
clean:
	rm -f $(OUT) $(SIG)
