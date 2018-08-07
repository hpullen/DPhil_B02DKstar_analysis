CC = g++
CFLAG = -Wall -Wpedantic -std=c++11 -g
INC = -I/home/pullen/analysis/B02DKstar/Libraries/CutReader/inc

ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats
CRLIB = /home/pullen/analysis/B02DKstar/Libraries/CutReader/libCutReader.so

all : eff

eff : GetEfficiencies

obj/GetEfficiencies.o : src/GetEfficiencies.cpp
	$(CC) $(INC) $(CFLAGS) $(ROOTINC) -c -o $@ $<

GetEfficiencies : obj/GetEfficiencies.o
	$(CC) $(ROOTLIB) $(CRLIB) -o $@ $<
