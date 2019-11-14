# Compiler settings
CC = g++
CFLAGS = -Wall -Wpedantic -g3 -std=c++11

# ROOT library linking
ROOTINC = `root-config --cflags` 
ROOTLIB = `root-config --libs` -lTMVA

# Plotting options
PLOTSTYLELIB = /home/pullen/analysis/B02DKstar/Libraries/plotStyle/libPlotStyle.so

# Directories
ODIR = obj
SDIR = src
INC = -Iinc -I/home/pullen/analysis/B02DKstar/Libraries/plotStyle/inc 

# Apply MVA
APPLY_SOURCES = Apply/main.cpp Apply/MVAApplicator.cpp
APPLY_OBJS = $(patsubst %, $(ODIR)/%, $(APPLY_SOURCES:.cpp=.o))
APPLY_OUT = ApplyMVA

# Rules 
.PHONY: all apply 

all: apply

apply : $(APPLY_OUT)

# Rules for executables
$(APPLY_OUT) : $(APPLY_OBJS)
	$(CC) -o $@ $^ $(ROOTLIB) $(ROOFITLIB) $(PLOTSTYLELIB) $(PDFLIB) $(CRUIJFFLIB)

# General rule for objects
$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CC) -c $(INC) $(CFLAGS) $(ROOTINC) $(LHCB_STYLE_INC) -o $@ $<

# Clean
clean : 
	rm obj/*.o
