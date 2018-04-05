CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats

PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
INC = -Iinc -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc -I/home/pullen/analysis/B02DKstar/Libraries/pdfs/inc

OUT = Charmless

.PHONY: all charmless

all: charmless

charmless:
	$(CC) src/charmless.cpp -o $(OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(INC) $(PLOTSTYLELIB)

.PHONY: clean
clean:
	rm -f $(OUT)
