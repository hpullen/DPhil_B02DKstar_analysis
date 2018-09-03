#!/usr/bin/env bash
INPUT_STRING="-i"
INPUT_STRING="$INPUT_STRING -c Kpi ../PID/Results/Kpi_B0_doubleSwap.param ../PID/Results/Kpi_B0bar_doubleSwap.param"
INPUT_STRING="$INPUT_STRING -c Kpipipi ../PID/Results/Kpipipi_B0_doubleSwap.param ../PID/Results/Kpipipi_B0bar_doubleSwap.param"
./AverageEfficiency ../Values/PID_efficiency_doubleSwap_run1.param $INPUT_STRING --run1
./AverageEfficiency ../Values/PID_efficiency_doubleSwap_run2.param $INPUT_STRING --run2
