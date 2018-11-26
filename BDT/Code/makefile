CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 -O3

# ROOT library linking
ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lTMVA
PLOTSTYLELIB = /home/pullen/analysis/tuple_scripts/analysis_code/Library/plotStyle/libPlotStyle.so

ODIR = obj
SDIR = src
INC = -Iinc -I/home/pullen/analysis/tuple_scripts/analysis_code/Library/pdfs/inc -I/home/pullen/analysis/tuple_scripts/analysis_code//Library/plotStyle/inc

# List sources
ROC_SOURCES = CompareRoc/main_roc.cpp CompareRoc/CompareRoc.cpp
TRAIN_SOURCES = Train/main.cpp Train/MVATrainer.cpp
APPLY_SOURCES = Apply/main.cpp Apply/MVAApplicator.cpp

_ROC_OBJS = $(ROC_SOURCES:.cpp=.o)
_TRAIN_OBJS = $(TRAIN_SOURCES:.cpp=.o)
_APPLY_OBJS = $(APPLY_SOURCES:.cpp=.o)
ROC_OBJS = $(patsubst %, $(ODIR)/%, $(_ROC_OBJS))
TRAIN_OBJS = $(patsubst %, $(ODIR)/%, $(_TRAIN_OBJS))
APPLY_OBJS = $(patsubst %, $(ODIR)/%, $(_APPLY_OBJS))

ROC_OUT = CompareRoc
TRAIN_OUT = TrainMVA
APPLY_OUT = ApplyMVA

.PHONY: all roc train apply 

all: roc train apply 

roc: $(ROC_OUT)

train: $(TRAIN_OUT)

apply: $(APPLY_OUT)

debug: CFLAGS += -DDEBUG
debug: $(OUT)

$(ROC_OUT): $(ROC_OBJS)
	$(CC) -o $(ROC_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB)

$(TRAIN_OUT): $(TRAIN_OBJS)
	$(CC) -o $(TRAIN_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB)

$(APPLY_OUT): $(APPLY_OBJS)
	$(CC) -o $(APPLY_OUT) $^ $(ROOTLIB) $(PLOTSTYLELIB)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c $(INC) $(ROOTINC) -o $@ $< $(CFLAGS)
# $@ = file name of target of rule
# $< = name of first prerequisite

.PHONY : clean
clean:
	rm -f $(ODIR)/*.o $(ODIR)/CompareRoc/*.o $(ODIR)/Train/*.o $(ODIR)/Apply/*.o $(ROC_OUT) $(TRAIN_OUT) $(APPLY_OUT) 
