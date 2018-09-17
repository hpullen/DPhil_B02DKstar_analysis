#!/usr/bin/env bash
INPUT_STRING="-i Kpi ../PID/Results/rho.param -c Kpipipi ../PID/Results/rho_Kpipipi_B0.param ../PID/Results/rho_Kpipipi_B0bar.param"
./AverageEfficiency ../Values/PID_efficiency_rho_run1.param $INPUT_STRING --run1
./AverageEfficiency ../Values/PID_efficiency_rho_run2.param $INPUT_STRING --run2

