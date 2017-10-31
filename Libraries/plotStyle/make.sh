g++ -c -Wall -Werror -fpic -Iinc `root-config --libs` `root-config --cflags` -lRooFit -lRooStats src/PlotStyle.cpp

g++ -shared -o libPlotStyle.so PlotStyle.o

mv *.o obj/
