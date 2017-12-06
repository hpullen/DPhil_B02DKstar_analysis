CC = g++ -g
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
TWOBODY_SOURCES = fitTwoBody.cpp ParameterReader.cpp Plotter.cpp ShapeMaker.cpp
FOURBODY_SOURCES = fitFourBody.cpp ParameterReader.cpp Plotter.cpp
TOY_SOURCES = fitTwoBodyToys.cpp ParameterReader.cpp ShapeMaker.cpp
LIKELIHOOD_SOURCES = toyLikelihood.cpp ParameterReader.cpp ShapeMaker.cpp Plotter.cpp

# Objects
_TWOBODY_OBJS = $(TWOBODY_SOURCES:.cpp=.o)
_FOURBODY_OBJS = $(FOURBODY_SOURCES:.cpp=.o)
_TOY_OBJS = $(TOY_SOURCES:.cpp=.o)
_LIKELIHOOD_OBJS = $(LIKELIHOOD_SOURCES:.cpp=.o)
TWOBODY_OBJS = $(patsubst %, $(ODIR)/%, $(_TWOBODY_OBJS))
FOURBODY_OBJS = $(patsubst %, $(ODIR)/%, $(_FOURBODY_OBJS))
TOY_OBJS = $(patsubst %, $(ODIR)/%, $(_TOY_OBJS))
LIKELIHOOD_OBJS = $(patsubst %, $(ODIR)/%, $(_LIKELIHOOD_OBJS))

# Output binary names
TWOBODY_OUT = FitTwoBody
FOURBODY_OUT = FitFourBody
TOY_OUT = ToysTwoBody
LIKELIHOOD_OUT = ToyLikelihood

.PHONY: all two four toys likelihood

all: two four toys likelihood

two: $(TWOBODY_OUT)

four: $(FOURBODY_OUT)

toys: $(TOY_OUT)

likelihood: $(LIKELIHOOD_OUT)

$(TWOBODY_OUT) : $(TWOBODY_OBJS)
	$(CC) -o $(TWOBODY_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB) $(PDFLIB)

$(FOURBODY_OUT) : $(FOURBODY_OBJS)
	$(CC) -o $(FOURBODY_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB) $(PDFLIB)

$(TOY_OUT) : $(TOY_OBJS)
	$(CC) -o $(TOY_OUT) $^ $(ROOTLIB) $(PDFLIB)

$(LIKELIHOOD_OUT) : $(LIKELIHOOD_OBJS)
	$(CC) -o $(LIKELIHOOD_OUT) $^ $(ROOTLIB) $(PDFLIB) $(PLOTSTYLELIB)

$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CC) -c $(INC) $(ROOTINC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f obj/* $(TWOBODY_OUT) $(FOURBODY_OUT) $(TOY_OUT) $(LIKELIHOOD_OUT)
