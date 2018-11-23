#!/usr/bin/env bash
# Script to make all PIDCalib ntuples for signal samples

# Loop over modes
for MODE in "Kpi" "KK" "pipi" "Kpipipi" "pipipipi"; do

    # Get years and flavours to loop over
    if [[ $MODE == "Kpipipi" || $MODE == "pipipipi" ]]; then
        FLAVS="B0 B0bar"
        if [[ $MODE == "Kpipipi" ]]; then
            YEARS="2012 2016"
        else 
            YEARS="2016"
        fi
    else 
        FLAVS="combined B0 B0bar"
        if [[ $MODE == "Kpi" ]]; then
            YEARS="2011 2012 2015 2016"
        else 
            YEARS="2012 2015 2016"
        fi
    fi

    # Loop
    for FLAV in $FLAVS; do
        for YEAR in $YEARS; do
            for MAG in MagUp MagDown; do
                ./apply_calib.sh $YEAR $MAG $MODE $FLAV
            done
        done
    done
done
