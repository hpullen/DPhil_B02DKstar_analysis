# Compiler settings
CC = g++
CFLAGS = -Wall -Wpedantic -g -std=c++11

# ROOT library linking
ROOTINC = `root-config --cflags` 
ROOTLIB = `root-config --libs`

# RooFit library linking
ROOFITLIB = -lRooStats -lRooFit

# Plotting options
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so
PDFLIB = /home/pullen/analysis/B02DKstar/Libraries/pdfs/libPdfs.so

# Directories
ODIR = obj
SDIR = src
INC = -Iinc/ShapeMaker -Iinc/Managers -Iinc/Fitters -Iinc/Plotter -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc -Iinc/Reader -I/home/pullen/analysis/B02DKstar/Libraries/pdfs/inc

# List of sources
# TEST_SOURCES = Tests/test_ToyMaker.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/SimpleShapeMakerBase.cpp ShapeMaker/SimpleToyMaker.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp
# TEST_SOURCES = Tests/test_paramReader.cpp Reader/ParameterReader.cpp
# TEST_SOURCES = FitTwoBody.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/TwoBodyBase.cpp ShapeMaker/TwoBodyPdfMaker.cpp Plotter/Plotter.cpp Reader/ParameterReader.cpp Fitters/DataFitter.cpp Fitters/TwoBodyFitter.cpp
# TEST_SOURCES = FitTwoAndFourBody.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/TwoAndFourBodyBase.cpp ShapeMaker/TwoAndFourBodyPdfMaker.cpp Plotter/Plotter.cpp Reader/ParameterReader.cpp Fitters/DataFitter.cpp Fitters/TwoAndFourBodyFitter.cpp ShapeMaker/TwoBodyBase.cpp

# Standard fit to data
DATA_SOURCES = FitTwoAndFourBody.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/TwoAndFourBodyBase.cpp ShapeMaker/TwoAndFourBodyPdfMaker.cpp Plotter/Plotter.cpp ShapeMaker/TwoBodyBase.cpp Reader/ParameterReader.cpp Fitters/DataFitter.cpp Fitters/TwoAndFourBodyFitter.cpp
DATA_OBJS = $(patsubst %, $(ODIR)/%, $(DATA_SOURCES:.cpp=.o))
DATA_OUT = FitData

# Fitter bias pulls production
FITTERBIAS_SOURCES = FitterBias.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/TwoAndFourBodyBase.cpp ShapeMaker/TwoAndFourBodyToyMaker.cpp ShapeMaker/TwoAndFourBodyPdfMaker.cpp Fitters/ToyFitter.cpp Plotter/Plotter.cpp ShapeMaker/TwoBodyBase.cpp Reader/ParameterReader.cpp ShapeMaker/SimpleShapeMakerBase.cpp ShapeMaker/SimplePdfMaker.cpp ShapeMaker/SimpleToyMaker.cpp
FITTERBIAS_OBJS = $(patsubst %, $(ODIR)/%, $(FITTERBIAS_SOURCES:.cpp=.o))
FITTERBIAS_OUT = FitterBias

# Rules 
.PHONY: all data bias

all: data bias

data : $(DATA_OUT)
bias : $(FITTERBIAS_OUT)

# Rules for executables
$(DATA_OUT) : $(DATA_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(FITTERBIAS_OUT) : $(FITTERBIAS_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)

# General rule for objects
$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CC) -c $(INC) $(CFLAGS) $(ROOTINC) -o $@ $<

# Clean
clean : 
	rm obj/*/*.o
