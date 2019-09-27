#!/usr/bin/env bash 
g++ -c -Wall -Werror -fpic -Iinc `root-config --libs` `root-config --cflags` -lRooFit -lRooStats src/RooHILLdini.cpp src/RooHORNSdini.cpp src/RooLITTLEHORNSdini.C
g++ -c -Wall -Werror -fpic -Iinc `root-config --libs` `root-config --cflags` -lRooFit -lRooStats src/RooHILLdini_misID.cpp src/RooHORNSdini_misID.cpp
g++ -c -Wall -Werror -fpic -Iinc `root-config --libs` `root-config --cflags` -lRooFit -lRooStats src/RooCruijff.cc

g++ -shared -o libPdfs.so RooHILLdini.o RooHORNSdini.o RooLITTLEHORNSdini.o RooCruijff.o RooHILLdini_misID.o RooHORNSdini_misID.o

mv *.o obj/
