#!/usr/bin/env bash

# Number of toy jobs to run
if [[ $# == 0 ]]; then
    echo Usage: ./run_many_fitterBias_toys.sh
    exit 1
fi
N_JOBS=$1

# Move old files to old folder
# echo "Deleting old files... "
# rm ../Results/FitterBias/old/*.root
# echo "Moving most recent files to old/ "
# mv ../Results/FitterBias/*.root ../Results/FitterBias/old/

# Loop through and submit
for i in $(seq 1 1 $N_JOBS); do
    sed "s/TOYNUMBER/${i}/" Templates/run_fitterBias_toy.sh > Jobs/run_fitterBias_toy_${i}.sh
    qsub -q veryshort -e Jobs/Outputs/ -o Jobs/Outputs/ Jobs/run_fitterBias_toy_${i}.sh
done
