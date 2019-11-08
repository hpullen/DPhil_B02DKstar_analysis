#!/usr/bin/env bash
# Make PIDCalib ntuples for misidentified B0 -> D*rho MC
for PARENT in "Bs_" ""; do 
    for MAG in MagDown MagUp; do
        for YEAR in 2012 2016; do
            ./apply_calib.sh $YEAR $MAG Kpi combined "${PARENT}lowMass"
        done
    done
done
