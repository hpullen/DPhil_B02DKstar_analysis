# Compiler settings
CC = g++
CFLAGS = -Wall -Wpedantic -g3 -std=c++11

# ROOT library linking
ROOTINC = `root-config --cflags` 
ROOTLIB = `root-config --libs`

# RooFit library linking
ROOFITLIB = -lRooStats -lRooFit -lRooFitCore

# Plotting options
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
PDFLIB = /home/pullen/analysis/B02DKstar/Libraries/pdfs/libPdfs.so
CRUIJFFLIB = /home/pullen/analysis/B02DKstar/Libraries/pdfs/Cruijff/libCruijff.so

# Directories
ODIR = obj
SDIR = src
INC = -Iinc -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc -Iinc/Reader -I/home/pullen/analysis/B02DKstar/Libraries/B2DHPartRecoPDFs/inc -I/home/pullen/analysis/B02DKstar/Libraries/pdfs/Cruijff/inc
LHCB_STYLE_INC = -I/home/pullen/analysis/B02DKstar/Libraries/

# Signal MC fit
SIGNAL_SOURCES = fitSignal.cpp Plotter.cpp
SIGNAL_OBJS = $(patsubst %, $(ODIR)/%, $(SIGNAL_SOURCES:.cpp=.o))
SIGNAL_OUT = FitSignal

# Low mass MC fit
LOW_SOURCES = fitLowMass.cpp Plotter.cpp
LOW_OBJS = $(patsubst %, $(ODIR)/%, $(LOW_SOURCES:.cpp=.o))
LOW_OUT = FitLowMass

# Rules 
.PHONY: all signal low

all: signal low

signal : $(SIGNAL_OUT)
low : $(LOW_OUT)

# Rules for executables
$(SIGNAL_OUT) : $(SIGNAL_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB) $(CRUIJFFLIB)
$(LOW_OUT) : $(LOW_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB) $(CRUIJFFLIB)

# General rule for objects
$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CC) -c $(INC) $(CFLAGS) $(ROOTINC) $(LHCB_STYLE_INC) -o $@ $<

# Clean
clean : 
	rm obj/*.o
