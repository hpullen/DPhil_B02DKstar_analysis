#!/usr/bin/env bash
if [[ $# == 1 ]]; then
    echo Using particle $1
    PARTICLE=$1
    NORMAL=0
else 
    NORMAL=1
fi
INPUT_STRING="-i"
for MODE in Kpi KK pipi; do
    if [[ $PARTICLE == "B0" ]]; then
        INPUT_STRING="$INPUT_STRING $MODE ../PID/Results/${MODE}_B0.param"
    elif [[ $PARTICLE == "B0bar" ]]; then
        INPUT_STRING="$INPUT_STRING $MODE ../PID/Results/${MODE}_B0bar.param"
    else 
        INPUT_STRING="$INPUT_STRING $MODE ../PID/Results/${MODE}.param"
    fi
done
if [[ $NORMAL  == 1 ]]; then
    INPUT_STRING="$INPUT_STRING -c"
fi
for MODE in Kpipipi pipipipi; do
    if [[ $PARTICLE == "B0" ]]; then
        INPUT_STRING="$INPUT_STRING $MODE ../PID/Results/${MODE}_B0.param"
    elif [[ $PARTICLE == "B0bar" ]]; then
        INPUT_STRING="$INPUT_STRING $MODE ../PID/Results/${MODE}_B0bar.param"
    else 
        INPUT_STRING="$INPUT_STRING $MODE ../PID/Results/${MODE}_B0.param ../PID/Results/${MODE}_B0bar.param"
    fi
done
echo Input string: $INPUT_STRING
if [[ $PARTICLE == "B0" ]]; then
    ./AverageEfficiency ../Values/PID_efficiency_B0.param $INPUT_STRING
    ./AverageEfficiency ../Values/PID_efficiency_B0_run1.param $INPUT_STRING --run1
    ./AverageEfficiency ../Values/PID_efficiency_B0_run2.param $INPUT_STRING --run2
elif [[ $PARTICLE == "B0bar" ]]; then
    ./AverageEfficiency ../Values/PID_efficiency_B0bar.param $INPUT_STRING
    ./AverageEfficiency ../Values/PID_efficiency_B0bar_run1.param $INPUT_STRING --run1
    ./AverageEfficiency ../Values/PID_efficiency_B0bar_run2.param $INPUT_STRING --run2
else 
    ./AverageEfficiency ../Values/PID_efficiency.param $INPUT_STRING
    ./AverageEfficiency ../Values/PID_efficiency_run1.param $INPUT_STRING --run1
    ./AverageEfficiency ../Values/PID_efficiency_run2.param $INPUT_STRING --run2
fi
