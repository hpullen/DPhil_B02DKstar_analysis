#!/usr/bin/env bash 
# Make PIDCalib ntuples with doubly misidentified D daughters
for MAG in MagDown MagUp; do
    
    # Kpi: combined flavour, every year
    for YEAR in 2011 2012 2015 2016; do
        ./apply_calib.sh $YEAR $MAG Kpi combined doubleSwap_Kstar
    done

done
