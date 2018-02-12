#!/usr/bin/env bash

# Script to run one toy study job (= 4 toys)
# Job ID
JOB_ID=$1

# Run the job
cd /home/pullen/analysis/B02DKstar/Fit_data/Macros/for_Malcolm
root -b -q -l "FitToy.C(\"Y\", \"$JOB_ID\")"


