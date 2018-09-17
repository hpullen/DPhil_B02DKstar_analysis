#!/usr/bin/env bash
INPUT_STRING="-i Kpi ../Generator/rho/Kpi.param -i Kpipipi ../Generator/rho/Kpipipi.param"
./AverageEfficiency ../Values/acceptance_efficiency_rho_run1.param $INPUT_STRING --run1
./AverageEfficiency ../Values/acceptance_efficiency_rho_run2.param $INPUT_STRING --run2
