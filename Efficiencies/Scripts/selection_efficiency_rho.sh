#!/usr/bin/env bash
INPUT_STRING="-i"
INPUT_STRING="$INPUT_STRING Kpi ../Selection/Results/selection_efficiency_rho.txt"
INPUT_STRING="$INPUT_STRING Kpipipi ../Selection/Results/selection_efficiency_rho_Kpipipi.txt"
./AverageEfficiency ../Values/selection_efficiency_rho_run1.param $INPUT_STRING --run1
./AverageEfficiency ../Values/selection_efficiency_rho_run2.param $INPUT_STRING --run2
