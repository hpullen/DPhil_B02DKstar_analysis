CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

# ROOT library linking
ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats

# Plotting style library
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so

ODIR = obj
SDIR = src
INC = -Iinc -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc

# List sources
SIGNAL_SOURCES = fitSignal.cpp Plotter.cpp

# Objects
_SIGNAL_OBJS = $(SIGNAL_SOURCES:.cpp=.o)
SIGNAL_OBJS = $(patsubst %, $(ODIR)/%, $(_SIGNAL_OBJS))

# Output binary names
SIGNAL_OUT = FitSignal

.PHONY: all signal

all: signal

signal: $(SIGNAL_OUT)

$(SIGNAL_OUT) : $(SIGNAL_OBJS)
	$(CC) -o $(SIGNAL_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c $(INC) $(ROOTINC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(SIGNAL_OUT)
