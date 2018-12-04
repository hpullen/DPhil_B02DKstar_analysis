CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 -O3

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats

PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
INC = -Iinc -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc -I/home/pullen/analysis/B02DKstar/Libraries/pdfs/inc

CUTINC = -I/home/pullen/analysis/B02DKstar/Libraries/CutReader/inc
CUTLIB = /home/pullen/analysis/B02DKstar/Libraries/CutReader/libCutReader.so

OUT = Charmless
CHECK_OUT=CheckSidebands
SWAP_OUT=SwapEfficiency

.PHONY: all charmless check swap

all: charmless check swap

charmless:
	$(CC) src/charmless.cpp -o $(OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(INC) $(PLOTSTYLELIB) $(CUTINC) $(CUTLIB)

check:
	$(CC) src/background_check.cpp -o $(CHECK_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(INC) $(PLOTSTYLELIB) $(CUTINC) $(CUTLIB)

swap:
	$(CC) src/swap_efficiencies.cpp -o $(SWAP_OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(INC) $(PLOTSTYLELIB) $(CUTINC) $(CUTLIB)

.PHONY: clean
clean:
	rm -f $(OUT)
