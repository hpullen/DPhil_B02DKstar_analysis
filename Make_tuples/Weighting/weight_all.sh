#!/usr/bin/env bash 
for BOD in 2 4; do
    for YEAR in 2011 2012 2015 2016; do
        python apply_weights.py $YEAR $BOD $*
    done
done
