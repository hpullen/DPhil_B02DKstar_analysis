#!/usr/bin/env bash 
for mode in "Kpi" "piK" "KK" "pipi" "Kpipipi" "piKpipi"; do
    ./Charmless $mode 1 $*
    ./Charmless $mode 2 $*
    # ./Charmless $mode both
done
./Charmless "pipipipi" 2 $*
