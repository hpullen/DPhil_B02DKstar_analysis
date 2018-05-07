#!/usr/bin/env bash 

for YEAR in 2011 2012 2015 2016; do
    for MAG in up down; do
        ./get_asymmetry.sh $YEAR $MAG D0
    done
done
