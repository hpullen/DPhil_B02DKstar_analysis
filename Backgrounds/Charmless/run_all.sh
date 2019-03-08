#!/usr/bin/env bash 
for mode in "Kpi" "piK" "KK" "pipi" "Kpipipi" "piKpipi"; do
    # ./Charmless $mode 1 $*
    # ./Charmless $mode 2 $*
    ./Charmless $mode both --high_stats
done
# ./Charmless "pipipipi" 2 $*

# Make files with yield after cut only
# cd Results/$*
# for FILE in *.txt; do
    # awk '/^3/{print $2, $3, $4}' $FILE > after_cut/$FILE
# done
