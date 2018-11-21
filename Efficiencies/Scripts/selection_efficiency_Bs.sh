#!/usr/bin/env bash
INPUT_STRING="-i"
INPUT_STRING="$INPUT_STRING Kpi ../Selection/Results/selection_efficiency_Bs.txt"
./AverageEfficiency ../Values/selection_efficiency_Bs_run1.param $INPUT_STRING --run1
./AverageEfficiency ../Values/selection_efficiency_Bs_run2.param $INPUT_STRING --run2
./total_efficiency.sh --Bs
