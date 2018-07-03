#!/usr/bin/env bash
INPUT_STRING="-i Kpi ../PID/Results/rho.param"
./AverageEfficiency ../Values/PID_efficiency_rho_run1.param $INPUT_STRING --run1
./AverageEfficiency ../Values/PID_efficiency_rho_run2.param $INPUT_STRING --run2

