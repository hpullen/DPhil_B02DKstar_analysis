CC = g++
CFLAGS = -Wall -Werror -fpic 
ROOTFLAGS = `root-config --cflags`
ROOLIBS = `root-config --libs` -lRooFit -lRooStats
INC = -Iinc

libPlotStyle.so : obj/PlotStyle.o
	$(CC) -shared -o libPlotStyle.so obj/PlotStyle.o

obj/PlotStyle.o : src/PlotStyle.cpp
	$(CC) -c $(CFLAGS) $(ROOTFLAGS) $(ROOTLIBS) $(INC) -o obj/PlotStyle.o src/PlotStyle.cpp

