#!/usr/bin/env bash 
for MODE in Kpi Kpipipi KK pipi pipipipi; do
    for YEAR in 2017 2018; do
        for MAG in up down; do
            ./SWeightTuple $YEAR $MAG $MODE
        done
    done
done
