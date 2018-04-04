#/usr/bin/bash

for year in 2011 2012 2015 2016; do
    for mag in up down; do
        ./MakeSelectedTuple $year $mag $1 --fullD
    done
done


