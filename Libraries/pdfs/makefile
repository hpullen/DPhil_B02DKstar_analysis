CC = g++
CFLAGS = -Wall -Wpedantic -g -std=c++11
ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs` -lRooFit -lRooStats

ODIR = obj
SDIR = src
INC = -Iinc

# List sources
PDF_SOURCES = RooHILLdini.cpp RooHORNSdini.cpp RooLITTLEHORNSdini.cpp RooHILLdini_misID.cpp RooHORNSdini_misID.cpp RooCruijff.cpp
# PDF_SOURCES = RooHILLdini.cpp RooHORNSdini.cpp RooLITTLEHORNSdini.cpp RooCruijff.cpp

# Objects
_PDF_OBJS = $(PDF_SOURCES:.cpp=.o)
PDF_OBJS  = $(patsubst %, $(ODIR)/%, $(_PDF_OBJS))

.PHONY: all
all : pdfs

pdfs : $(PDF_OBJS)
	$(CC) -shared -o libPdfs.so $^ $(ROOTLIB)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c -fPIC $(INC) $(ROOTINC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f obj/* libPdfs.so
