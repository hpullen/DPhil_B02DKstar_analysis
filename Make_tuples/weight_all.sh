#!/usr/bin/env bash 
for MODE in Kpi Kpipipi KK pipi pipipipi; do
    for YEAR in 2011 2012 2015 2016; do
        for MAG in up down; do
            ./WeightTuple $YEAR $MAG $MODE
        done
    done
done
