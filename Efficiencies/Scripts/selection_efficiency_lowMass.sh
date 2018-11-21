#!/usr/bin/env bash 
# Average over years
VAR_LIST="-i"
for MODE in {gamma,pi}_{101,010}; do
    VAR_LIST="${VAR_LIST} $MODE ../Selection/Results/LowMass/${MODE}.param"
done
./AverageEfficiency ../Values/selection_lowMass.param $VAR_LIST
./AverageEfficiency ../Values/selection_lowMass_run1.param $VAR_LIST --run1
./AverageEfficiency ../Values/selection_lowMass_run2.param $VAR_LIST --run2

