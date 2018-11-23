#!/usr/bin/env bash
# Make PIDCalib ntuples for misidentified B0 -> Drho MC
for MAG in MagDown MagUp; do
    for YEAR in 2012 2016; do
        ./apply_calib.sh $YEAR $MAG Kpi combined rho
    done
done
