#!/usr/bin/env bash

# Number of toy jobs to run
N_JOBS=$1

# Loop through and submit
for i in $(seq 1 1 $N_JOBS); do
    sed "s/TOYNUMBER/${i}/" Templates/run_fitterBias_toy.sh > Jobs/run_fitterBias_toy_${i}.sh
    qsub -q veryshort -e Jobs/Outputs/ -o Jobs/Outputs/ Jobs/run_fitterBias_toy_${i}.sh
done
