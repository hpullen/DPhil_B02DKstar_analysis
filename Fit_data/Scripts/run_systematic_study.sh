#!/usr/bin/env bash

# Number of jobs to run
if [[ $# == 0 ]]; then
    echo "Usage: ./run_significance_study.sh <type>"
    exit 1
fi
N_JOBS=$1
TYPE=$2
EXTRAS=$3

# Check output folder exists
DIR="/data/lhcb/users/pullen/B02DKstar/systematics/"
echo Extras: $EXTRAS
DIR_NAME=$TYPE
if [[ $EXTRAS == "--combineRuns" ]]; then
    DIR_NAME="/CombinedRuns/$TYPE/"
fi
if [[ ! -d "$DIR/$DIR_NAME" ]]; then
    echo Making dir $DIR/$DIR_NAME
    mkdir $DIR/$DIR_NAME
fi

# Loop through and submit
for i in $(seq 1 1 $N_JOBS); do
    sed "s/NUMBER/${i}/; s/EXTRAOPTS/${TYPE} ${EXTRAS}/" Templates/run_systematics.sh > $DIR/Jobs/run_systematics_${TYPE}_${i}.sh
    qsub -lnodes=1:ppn=8 -e $DIR/Outputs/ -o $DIR/Outputs/ $DIR/Jobs/run_systematics_${TYPE}_${i}.sh
done
