#!/usr/bin/env bash 
g++ -c -Wall -Werror -fpic -Iinc `root-config --libs` `root-config --cflags` -lRooFit -lRooStats src/RooHILLdini.C src/RooHORNSdini.C src/RooLITTLEHORNSdini.C

g++ -shared -o libPdfs.so RooHILLdini.o RooHORNSdini.o RooLITTLEHORNSdini.o

mv *.o obj/
