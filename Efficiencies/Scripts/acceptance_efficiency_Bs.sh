#!/usr/bin/env bash
INPUT_STRING="-i Kpi ../Generator/Bs.param"
./AverageEfficiency ../Values/acceptance_efficiency_Bs_run1.param $INPUT_STRING --run1
./AverageEfficiency ../Values/acceptance_efficiency_Bs_run2.param $INPUT_STRING --run2
