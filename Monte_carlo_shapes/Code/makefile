CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 -g -O3

# ROOT library linking
ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats

# Library linking for fit library
PDFLIB = /home/pullen/analysis/B02DKstar/Libraries/pdfs/libPdfs.so
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so

ODIR = obj
SDIR = src
INC = -Iinc -I/home/pullen/analysis/B02DKstar/Libraries/pdfs/inc -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc

# List sources
SIGNAL_SOURCES = Signal/main.cpp Fitter.cpp DoubleCBFitter.cpp DoubleGaussianFitter.cpp RangeReader.cpp StartingParamReader.cpp StartingParam.cpp Signal/SignalData.cpp Signal/ProcessSignal.cpp
LOWMASS_SOURCES = LowMass/main.cpp LowMass/ProcessLowMassBackground.cpp LowMass/LowMassBackground.cpp LowMass/LowMassFitter.cpp RangeReader.cpp

_SIGNAL_OBJS = $(SIGNAL_SOURCES:.cpp=.o)
_LOWMASS_OBJS = $(LOWMASS_SOURCES:.cpp=.o)
SIGNAL_OBJS = $(patsubst %, $(ODIR)/%, $(_SIGNAL_OBJS))
LOWMASS_OBJS = $(patsubst %, $(ODIR)/%, $(_LOWMASS_OBJS))

SIGNAL_OUT = ProcessSignal
LOWMASS_OUT = ProcessLowMassBackgrounds

.PHONY: all signal lowmass

all: signal lowmass
signal: $(SIGNAL_OUT)
lowmass: $(LOWMASS_OUT)

$(SIGNAL_OUT): $(SIGNAL_OBJS)
	$(CC) -o $(SIGNAL_OUT) $^ $(ROOTLIB) $(PDFLIB) $(PLOTSTYLELIB)

$(LOWMASS_OUT): $(LOWMASS_OBJS)
	$(CC) -o $(LOWMASS_OUT) $^ $(ROOTLIB) $(PDFLIB) $(PLOTSTYLELIB)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c $(INC) $(ROOTINC) -o $@ $< $(CFLAGS)

.PHONY : clean
clean:
	rm -f $(ODIR)/*.o $(SIGNAL_OUT) $(LOWMASS_OUT)
