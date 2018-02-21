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
# TEST_SOURCES = Tests/test_ToyMaker.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/SimpleShapeMakerBase.cpp ShapeMaker/SimpleToyMaker.cpp ShapeMaker/SimplePdfMaker.cpp Fitters/ToyFitter.cpp Fitters/ToySignificanceFitter.cpp Plotter/Plotter.cpp
# TEST_SOURCES = Tests/test_paramReader.cpp Reader/ParameterReader.cpp
TEST_SOURCES = Tests/test_twoBody.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/TwoBodyBase.cpp ShapeMaker/TwoBodyPdfMaker.cpp Plotter/Plotter.cpp Reader/ParameterReader.cpp
TEST_OBJS = $(patsubst %, $(ODIR)/%, $(TEST_SOURCES:.cpp=.o))

# Output executables
TEST_OUT = Test

# Rules 
.PHONY: all test base

all: test base

test : $(TEST_OUT)

base: $(ODIR)/ShapeMaker/ShapeMakerBase.o

# simplebase: $(ODIR)/ShapeMaker/SimpleShapeMakerBase.o

# manager: $(ODIR)/Managers/ManagerBase.o

# param: $(ODIR)/Managers/ParameterManager.o

# Rules for executables
$(TEST_OUT) : $(TEST_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)

# General rule for objects
$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CC) -c $(INC) $(CFLAGS) $(ROOTINC) -o $@ $<

# Clean
clean : 
	rm obj/*/*.o
