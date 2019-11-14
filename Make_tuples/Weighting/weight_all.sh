#!/usr/bin/env bash 
for BOD in 2 4; do
    for YEAR in 2017 2018; do
        python apply_weights.py $YEAR $BOD $*
    done
done
