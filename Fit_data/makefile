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

# Crosscheck fit to data (floating yields)
CROSSCHECK_SOURCES = CrossCheck.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp Plotter/Plotter.cpp Reader/ParameterReader.cpp Fitters/DataFitter.cpp ShapeMaker/DataPdfMaker.cpp ShapeMaker/CrossCheckPdfMaker.cpp
CROSSCHECK_OBJS = $(patsubst %, $(ODIR)/%, $(CROSSCHECK_SOURCES:.cpp=.o))
CROSSCHECK_OUT = CrossCheck

# Plotting from ROOT fit
PLOT_SOURCES = PlotDataFit.cpp Plotter/Plotter.cpp
PLOT_OBJS = $(patsubst %, $(ODIR)/%, $(PLOT_SOURCES:.cpp=.o))
PLOT_OUT = PlotFit

# Fitter bias pulls production
FITTERBIAS_SOURCES = FitterBias.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/DataPdfMaker.cpp ShapeMaker/ToyPdfMaker.cpp Fitters/ToyFitter.cpp Plotter/Plotter.cpp ShapeMaker/TwoBodyBase.cpp Reader/ParameterReader.cpp ShapeMaker/SimpleShapeMakerBase.cpp ShapeMaker/SimplePdfMaker.cpp ShapeMaker/SimpleToyMaker.cpp
FITTERBIAS_OBJS = $(patsubst %, $(ODIR)/%, $(FITTERBIAS_SOURCES:.cpp=.o))
FITTERBIAS_OUT = FitterBias

# Significance estimation
SIGNIFICANCE_SOURCES = SignificanceTwoAndFourBody.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ShapeManager.cpp Managers/ParameterList.cpp ShapeMaker/ShapeMakerBase.cpp Fitters/ToyFitter.cpp Plotter/Plotter.cpp Reader/ParameterReader.cpp ShapeMaker/SimpleShapeMakerBase.cpp ShapeMaker/SimplePdfMaker.cpp ShapeMaker/SimpleToyMaker.cpp Fitters/ToySignificanceFitter.cpp ShapeMaker/DataPdfMaker.cpp ShapeMaker/ToyPdfMaker.cpp ShapeMaker/RunOneToyMaker.cpp
SIGNIFICANCE_OBJS = $(patsubst %, $(ODIR)/%, $(SIGNIFICANCE_SOURCES:.cpp=.o))
SIGNIFICANCE_OUT = Significance

# Systematic uncertainty evaluation
SYSTEMATIC_SOURCES = SystematicStudy.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ParameterList.cpp Managers/ShapeManager.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/TwoBodyBase.cpp ShapeMaker/TwoAndFourBodyBase.cpp ShapeMaker/DataPdfMaker.cpp ShapeMaker/SystematicPdfMaker.cpp Reader/ParameterReader.cpp Fitters/DataFitter.cpp Fitters/SystematicFitter.cpp
SYSTEMATIC_OBJS = $(patsubst %, $(ODIR)/%, $(SYSTEMATIC_SOURCES:.cpp=.o))
SYSTEMATIC_OUT = Systematics

# Fits with different BDT cuts
BDT_SOURCES = OptimiseBDT.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ParameterList.cpp Managers/ShapeManager.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/DataPdfMaker.cpp Reader/ParameterReader.cpp Fitters/DataFitter.cpp Fitters/TwoAndFourBodyFitter.cpp Fitters/CutFitter.cpp
BDT_OBJS = $(patsubst %, $(ODIR)/%, $(BDT_SOURCES:.cpp=.o))
BDT_OUT = StudyBDT
# BDT_SOURCES = OptimizeBDTv2.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ParameterList.cpp Managers/ShapeManager.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/DataPdfMaker.cpp ShapeMaker/ToyPdfMaker.cpp ShapeMaker/ToyBDTPdfMaker.cpp Fitters/ToyFitter.cpp Reader/ParameterReader.cpp
# BDT_OBJS = $(patsubst %, $(ODIR)/%, $(BDT_SOURCES:.cpp=.o))
# BDT_OUT = StudyBDT

# Toy fits with different BDT cuts
TOY_BDT_SOURCES = OptimiseBDT_toys.cpp Managers/ManagerBase.cpp Managers/ParameterManager.cpp Managers/ParameterList.cpp Managers/ShapeManager.cpp ShapeMaker/ShapeMakerBase.cpp ShapeMaker/DataPdfMaker.cpp ShapeMaker/ToyPdfMaker.cpp Reader/ParameterReader.cpp Fitters/ToyFitter.cpp
TOY_BDT_OBJS = $(patsubst %, $(ODIR)/%, $(TOY_BDT_SOURCES:.cpp=.o))
TOY_BDT_OUT = StudyBDTtoy

# Rules 
.PHONY: all data plt bias sig bdt toy_bdt sys crosscheck

# all: data plt bias sig bdt
all: data sys crosscheck bias plt bdt toy_bdt

data : $(DATA_OUT)
crosscheck : $(CROSSCHECK_OUT)
plt : $(PLOT_OUT)
bias : $(FITTERBIAS_OUT)
sig : $(SIGNIFICANCE_OUT)
sys : $(SYSTEMATIC_OUT)
bdt : $(BDT_OUT)
toy_bdt : $(TOY_BDT_OUT)

# Rules for executables
$(DATA_OUT) : $(DATA_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(CROSSCHECK_OUT) : $(CROSSCHECK_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(PLOT_OUT) : $(PLOT_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(FITTERBIAS_OUT) : $(FITTERBIAS_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(SIGNIFICANCE_OUT) : $(SIGNIFICANCE_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(SYSTEMATIC_OUT) : $(SYSTEMATIC_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(BDT_OUT) : $(BDT_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)
$(TOY_BDT_OUT) : $(TOY_BDT_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB)

# General rule for objects
$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CC) -c $(INC) $(CFLAGS) $(ROOTINC) -o $@ $<

# Clean
clean : 
	rm obj/*/*.o obj/*.o