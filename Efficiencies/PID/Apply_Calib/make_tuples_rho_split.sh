#!/usr/bin/env bash
# Make PIDCalib ntuples for misidentified B0 -> Drho MC
for MAG in MagDown MagUp; do
    for SIGN in B0 B0bar; do
        ./apply_calib.sh 2012 $MAG Kpi $SIGN rho
    done
done
