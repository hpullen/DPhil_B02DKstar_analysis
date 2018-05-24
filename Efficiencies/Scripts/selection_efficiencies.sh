#!/usr/bin/env bash
INPUT_STRING="-i"
for MODE in Kpi KK pipi Kpipipi pipipipi; do
    INPUT_STRING="$INPUT_STRING $MODE ../Selection/Results/${MODE}.param"
done
./AverageEfficiency ../Values/selection_efficiency.param $INPUT_STRING
./AverageEfficiency ../Values/selection_efficiency_run1.param $INPUT_STRING --run1
./AverageEfficiency ../Values/selection_efficiency_run2.param $INPUT_STRING --run2
