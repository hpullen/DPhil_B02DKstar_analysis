#!/usr/bin/env bash 
for RUN in 1 2; do
    for MODE in Kpi Kpipipi; do
        ./GetEfficiencies $MODE $RUN
    done
done
