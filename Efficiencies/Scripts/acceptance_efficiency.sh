#!/usr/bin/env bash
INPUT_STRING="-i"
for MODE in Kpi KK pipi Kpipipi pipipipi; do
    INPUT_STRING="$INPUT_STRING $MODE ../Generator/${MODE}.param"
done
./AverageEfficiency ../Values/acceptance_efficiency.param $INPUT_STRING
./AverageEfficiency ../Values/acceptance_efficiency_run1.param $INPUT_STRING --run1
./AverageEfficiency ../Values/acceptance_efficiency_run2.param $INPUT_STRING --run2
./AverageEfficiency ../Values/acceptance_efficiency_allYears.param $INPUT_STRING --allYears
