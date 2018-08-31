CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 -g

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

CUTINC = -I/home/pullen/analysis/B02DKstar/Libraries/CutReader/inc
CUTLIB = /home/pullen/analysis/B02DKstar/Libraries/CutReader/libCutReader.so

PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
PLOTSTYLEINC = -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc

OUT = PlotDKmass

.PHONY: all DKmass

all: DKmass

DKmass:
	$(CC) plot_DK_mass.cpp -o $(OUT) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(CUTINC) $(CUTLIB) $(PLOTSTYLEINC) $(PLOTSTYLELIB)

.PHONY: clean
	
clean:
	rm -f $(OUT)
