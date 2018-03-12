#!/usr/bin/env bash

# Number of toy jobs to run
N_JOBS=$1
MODE=$2

# Loop through and submit
for i in $(seq 1 1 $N_JOBS); do
    sed "s/TOYNUMBER/${i}/;s/MODE/$MODE/" Templates/run_significance_toy.sh > Jobs/run_significance_toy_${MODE}_${i}.sh
    qsub -q veryshort -e Jobs/Outputs/ -o Jobs/Outputs/ Jobs/run_significance_toy_${i}.sh
done
