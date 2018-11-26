CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 -g

# ROOT library linking
ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats

# Plotting style library
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
PDFLIB = /home/pullen/analysis/B02DKstar/Libraries/pdfs/libPdfs.so

ODIR = obj
SDIR = src
INC = -Iinc -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc -I/home/pullen/analysis/B02DKstar/Libraries/pdfs/inc

# List sources
SIGNAL_SOURCES = fitSignal.cpp Plotter.cpp
SIGNAL_SIM_SOURCES = fitSignalSim.cpp Plotter.cpp
LOWMASS_SOURCES = fitLowMass.cpp Plotter.cpp
RHO_SOURCES = fitRho.cpp Plotter.cpp
GAUSS_SOURCES = findWidthRatio.cpp Plotter.cpp
DKPIPI_SOURCES = fitDKpipi.cpp Plotter.cpp

# Objects
_SIGNAL_OBJS = $(SIGNAL_SOURCES:.cpp=.o)
SIGNAL_OBJS = $(patsubst %, $(ODIR)/%, $(_SIGNAL_OBJS))
_SIGNAL_SIM_OBJS = $(SIGNAL_SIM_SOURCES:.cpp=.o)
SIGNAL_SIM_OBJS = $(patsubst %, $(ODIR)/%, $(_SIGNAL_SIM_OBJS))
_LOWMASS_OBJS = $(LOWMASS_SOURCES:.cpp=.o)
LOWMASS_OBJS = $(patsubst %, $(ODIR)/%, $(_LOWMASS_OBJS))
_RHO_OBJS = $(RHO_SOURCES:.cpp=.o)
RHO_OBJS = $(patsubst %, $(ODIR)/%, $(_RHO_OBJS))
_GAUSS_OBJS = $(GAUSS_SOURCES:.cpp=.o)
GAUSS_OBJS = $(patsubst %, $(ODIR)/%, $(_GAUSS_OBJS))
_DKPIPI_OBJS = $(DKPIPI_SOURCES:.cpp=.o)
DKPIPI_OBJS = $(patsubst %, $(ODIR)/%, $(_DKPIPI_OBJS))

# Output binary names
SIGNAL_OUT = FitSignal
SIGNAL_SIM_OUT = FitSignalSim
LOWMASS_OUT = FitLowMass
RHO_OUT = FitRho
GAUSS_OUT = FindWidthRatio
DKPIPI_OUT = FitDKpipi

.PHONY: all signal low rho gauss dkpipi sim

all: signal low rho gauss dkpipi sim

signal: $(SIGNAL_OUT)

sim: $(SIGNAL_SIM_OUT)

low: $(LOWMASS_OUT)

rho: $(RHO_OUT)

gauss: $(GAUSS_OUT)

dkpipi: $(DKPIPI_OUT)

$(SIGNAL_OUT) : $(SIGNAL_OBJS)
	$(CC) -o $(SIGNAL_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB) $(PDFLIB)

$(SIGNAL_SIM_OUT) : $(SIGNAL_SIM_OBJS)
	$(CC) -o $(SIGNAL_SIM_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB)

$(LOWMASS_OUT) : $(LOWMASS_OBJS)
	$(CC) -o $(LOWMASS_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB) $(PDFLIB)

$(RHO_OUT) : $(RHO_OBJS)
	$(CC) -o $(RHO_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB) $(PDFLIB)

$(GAUSS_OUT) : $(GAUSS_OBJS)
	$(CC) -o $(GAUSS_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB) $(PDFLIB)

$(DKPIPI_OUT) : $(DKPIPI_OBJS)
	$(CC) -o $(DKPIPI_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB) $(PDFLIB) 

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c $(INC) $(ROOTINC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(SIGNAL_OUT) $(SIGNAL_SIM_OUT) $(LOWMASS_OUT) $(RHO_OUT) $(GAUSS_OUT) $(DKPIPI_OUT)
