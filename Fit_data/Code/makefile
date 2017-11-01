CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11

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
TWOBODY_SOURCES = fitTwoBody.cpp ParameterReader.cpp ParameterManager.cpp ShapeManager.cpp

# Objects
_TWOBODY_OBJS = $(TWOBODY_SOURCES:.cpp=.o)
TWOBODY_OBJS = $(patsubst %, $(ODIR)/%, $(_TWOBODY_OBJS))

# Output binary names
TWOBODY_OUT = FitTwoBody

.PHONY: all two

all: two

two: $(TWOBODY_OUT)

$(TWOBODY_OUT) : $(TWOBODY_OBJS)
	$(CC) -o $(TWOBODY_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB) $(PDFLIB)

$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CC) -c $(INC) $(ROOTINC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f obj/* $(TWOBODY_OUT)
