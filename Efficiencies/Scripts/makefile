CC = g++
CFLAGS = -Wall -Wpedantic -std=c++11 -g

OUT = AverageEfficiency

INC = -I/home/pullen/analysis/B02DKstar/Libraries/ParameterReader/inc
LIB = /home/pullen/analysis/B02DKstar/Libraries/ParameterReader/libParameterReader.so

.PHONY: all out

all: out

out:
	$(CC) src/averageEfficiency.cpp -o $(OUT) $(CFLAGS) $(INC) $(LIB) 

.PHONY: clean
clean:
	rm -f $(OUT)
