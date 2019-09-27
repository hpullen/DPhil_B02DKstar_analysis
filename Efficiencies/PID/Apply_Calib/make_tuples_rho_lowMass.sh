#!/usr/bin/env bash
# Make PIDCalib ntuples for misidentified B0 -> D*rho MC
for MAG in MagDown MagUp; do
    for YEAR in 2016; do
        ./apply_calib.sh $YEAR $MAG Kpi combined rho_lowMass
    done
done

