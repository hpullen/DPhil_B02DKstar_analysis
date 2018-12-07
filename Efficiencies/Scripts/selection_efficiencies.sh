#!/usr/bin/env bash
for EXT in "_weighted" ""; do
    INPUT_STRING="-i"
    for MODE in Kpi KK pipi Kpipipi pipipipi; do
        INPUT_STRING="$INPUT_STRING $MODE ../Selection/Results/Signal/${MODE}${EXT}.param"
    done
    ./AverageEfficiency ../Values/selection_efficiency${EXT}.param $INPUT_STRING
    ./AverageEfficiency ../Values/selection_efficiency_run1${EXT}.param $INPUT_STRING --run1
    ./AverageEfficiency ../Values/selection_efficiency_run2${EXT}.param $INPUT_STRING --run2
done
./total_efficiency.sh
