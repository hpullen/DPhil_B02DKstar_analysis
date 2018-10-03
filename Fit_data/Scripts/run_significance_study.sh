#!/usr/bin/env bash

# Number of jobs to run
if [[ $# == 0 ]]; then
    echo "Usage: ./run_significance_study.sh <type>"
    exit 1
fi
N_JOBS=$1
TYPE=$2

# Check output folder exists
DIR="/data/lhcb/users/pullen/B02DKstar/systematics/"
if [[ ! -d "$DIR/$TYPE" ]]; then
    mkdir $DIR/$TYPE
fi

# Loop through and submit
for i in $(seq 1 1 $N_JOBS); do
    sed "s/NUMBER/${i}/; s/EXTRAOPTS/${TYPE}/" Templates/run_systematics.sh > $DIR/Jobs/run_systematics_${TYPE}_${i}.sh
    qsub -e $DIR/Outputs/ -o $DIR/Outputs/ $DIR/Jobs/run_systematics_${TYPE}_${i}.sh
done
