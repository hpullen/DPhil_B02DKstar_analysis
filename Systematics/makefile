CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 -g3

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
PARINC = -I/home/pullen/analysis/B02DKstar/Libraries/ParameterReader/inc
PARLIB = /home/pullen/analysis/B02DKstar/Libraries/ParameterReader/libParameterReader.so

INC = -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc

OUT = PlotSystematics
FINAL = FinalResult

.PHONY: all out final

all: out final

out:
	$(CC) src/plotSystematics.cpp -o $(OUT) $(INC) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PLOTSTYLELIB)

final:
	$(CC) src/get_final_results.cpp -o $(FINAL) $(CFLAGS) $(ROOTINC) $(ROOTLIB) $(PARINC) $(PARLIB)


.PHONY: clean
clean:
	rm -f $(OUT) $(FINAL)
