CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
PDFLIB = /home/pullen/analysis/B02DKstar/Libraries/pdfs/libPdfs.so

INC = -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc -I/home/pullen/analysis/B02DKstar/Libraries/pdfs/inc

OUT = CompareLowMass

.PHONY: all out

all: out

out:
	$(CC) src/compareLowMassShapes.cpp -o $(OUT) $(INC) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PLOTSTYLELIB) $(PDFLIB)

.PHONY: clean
clean:
	rm -f $(OUT)
