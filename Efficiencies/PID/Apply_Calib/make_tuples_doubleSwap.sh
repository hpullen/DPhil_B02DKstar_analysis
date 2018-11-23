#!/usr/bin/env bash 
# Make PIDCalib ntuples with doubly misidentified D daughters
for MAG in MagDown MagUp; do
    
    # Kpi: combined flavour, every year
    for YEAR in 2011 2012 2015 2016; do
        ./apply_calib.sh $YEAR $MAG Kpi combined doubleSwap
    done

    # Kpipipi: split flavours, 2012 and 2016 only
    for FLAV in B0 B0bar; do
        for YEAR in 2012 2016; do
            ./apply_calib.sh $YEAR $MAG Kpipipi $FLAV doubleSwap
        done
    done
done
