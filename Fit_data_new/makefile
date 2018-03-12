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
DATA_SOURCES = FitTwoAndFourBody.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp Plotter/Plotter.cpp Reader/ParameterReader.cpp Fitters/DataFitter.cpp Fitters/TwoAndFourBodyFitter.cpp ShapeMaker/DataPdfMaker.cpp
DATA_OBJS = $(patsubst %, $(ODIR)/%, $(DATA_SOURCES:.cpp=.o))
DATA_OUT = FitData

# Plotting from ROOT fit
PLOT_SOURCES = PlotDataFit.cpp Plotter/Plotter.cpp
PLOT_OBJS = $(patsubst %, $(ODIR)/%, $(PLOT_SOURCES:.cpp=.o))
PLOT_OUT = PlotFit

# Fitter bias pulls production
FITTERBIAS_SOURCES = FitterBias.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/TwoAndFourBodyBase.cpp ShapeMaker/TwoAndFourBodyToyMaker.cpp ShapeMaker/TwoAndFourBodyPdfMaker.cpp Fitters/ToyFitter.cpp Plotter/Plotter.cpp ShapeMaker/TwoBodyBase.cpp Reader/ParameterReader.cpp ShapeMaker/SimpleShapeMakerBase.cpp ShapeMaker/SimplePdfMaker.cpp ShapeMaker/SimpleToyMaker.cpp
FITTERBIAS_OBJS = $(patsubst %, $(ODIR)/%, $(FITTERBIAS_SOURCES:.cpp=.o))
FITTERBIAS_OUT = FitterBias

# Significance estimation
SIGNIFICANCE_SOURCES = SignificanceTwoAndFourBody.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp Fitters/ToyFitter.cpp Plotter/Plotter.cpp Reader/ParameterReader.cpp ShapeMaker/SimpleShapeMakerBase.cpp ShapeMaker/SimplePdfMaker.cpp ShapeMaker/SimpleToyMaker.cpp Fitters/ToySignificanceFitter.cpp ShapeMaker/DataPdfMaker.cpp ShapeMaker/ToyPdfMaker.cpp
SIGNIFICANCE_OBJS = $(patsubst %, $(ODIR)/%, $(SIGNIFICANCE_SOURCES:.cpp=.o))
SIGNIFICANCE_OUT = Significance

# Significance estimation
SYSTEMATIC_SOURCES = SystematicStudy.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ParameterList.cpp Managers/ShapeManager.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/TwoBodyBase.cpp ShapeMaker/TwoAndFourBodyBase.cpp ShapeMaker/TwoAndFourBodyPdfMaker.cpp ShapeMaker/SystematicPdfMaker.cpp Reader/ParameterReader.cpp Fitters/DataFitter.cpp Fitters/SystematicFitter.cpp
SYSTEMATIC_OBJS = $(patsubst %, $(ODIR)/%, $(SYSTEMATIC_SOURCES:.cpp=.o))
SYSTEMATIC_OUT = Systematics

# Plot of DKpipi shifted within uncertainties
DKPIPI_SOURCES = PlotDKpipiVariation.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp Reader/ParameterReader.cpp
DKPIPI_OBJS = $(patsubst %, $(ODIR)/%, $(DKPIPI_SOURCES:.cpp=.o))
DKPIPI_OUT = PlotDKpipiVariation

# Rules 
.PHONY: all data plt bias sig sys dkpipi

all: data plt bias sig sys dkpipi

data : $(DATA_OUT)
plt : $(PLOT_OUT)
bias : $(FITTERBIAS_OUT)
sig : $(SIGNIFICANCE_OUT)
sys : $(SYSTEMATIC_OUT)
dkpipi : $(DKPIPI_OUT)

# Rules for executables
$(DATA_OUT) : $(DATA_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(PLOT_OUT) : $(PLOT_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(FITTERBIAS_OUT) : $(FITTERBIAS_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(SIGNIFICANCE_OUT) : $(SIGNIFICANCE_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(SYSTEMATIC_OUT) : $(SYSTEMATIC_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(DKPIPI_OUT) : $(DKPIPI_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)

# General rule for objects
$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CC) -c $(INC) $(CFLAGS) $(ROOTINC) -o $@ $<

# Clean
clean : 
	rm obj/*/*.o
