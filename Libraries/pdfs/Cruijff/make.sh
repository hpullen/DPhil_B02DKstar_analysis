#!/usr/bin/env bash 
g++ -c -Wall -Werror -fpic -Iinc `root-config --libs` `root-config --cflags` -lRooFit -lRooStats src/RooCruijff.cc

g++ -shared -o libCruijff.so RooCruijff.o

mv *.o obj/
