#!/usr/bin/env bash

# Number of toy jobs to run
if [[ $# == 0 ]]; then
    echo Usage: ./run_many_fitterBias_toys.sh
    exit 1
fi
N_JOBS=$1
shift
EXTRA_OPTS=$*

# Which queue to use
QUEUE="short"
EXTRA=""
if grep "binned" <<< "$EXTRA_OPTS" > /dev/null; then
    QUEUE="veryshort"
    EXTRA="/Binned/"
fi
if grep "combineRuns" <<< "$EXTRA_OPTS" > /dev/null; then
    EXTRA="${EXTRA}/CombinedRuns"
fi
if grep "high_stats" <<< "$EXTRA_OPTS" > /dev/null; then
    EXTRA="${EXTRA}/high_stats"
fi

# Move old files to old folder
NEW_DIR=/data/lhcb/users/pullen/B02DKstar/toys/FitterBias/${EXTRA}/
OLD_DIR=${NEW_DIR}/old
if [[ ! -z $(find $NEW_DIR -maxdepth 1 -name "*.root" -print -quit) ]]; then
    echo "Will move old files in $NEW_DIR"
    if [[ ! -z $(find $OLD_DIR -maxdepth 1 -name "*.root" -print -quit) ]]; then
        echo "Will delete older files in $OLD_DIR"
        rm /data/lhcb/users/pullen/B02DKstar/toys/FitterBias/${EXTRA}/old/*.root
    fi
    mv /data/lhcb/users/pullen/B02DKstar/toys/FitterBias/${EXTRA}/*.root /data/lhcb/users/pullen/B02DKstar/toys/FitterBias/${EXTRA}/old
fi

# Loop through and submit
LOGDIR="/data/lhcb/users/pullen/B02DKstar/toys/FitterBias/outputs/"
for i in $(seq 1 1 $N_JOBS); do
    sed "s/TOYNUMBER/${i}/; s/EXTRAOPTS/${EXTRA_OPTS}/" Templates/run_fitterBias_toy.sh > Jobs/run_fitterBias_toy_${i}.sh
    qsub -lnodes=1:ppn=8 -q $QUEUE -e $LOGDIR -o $LOGDIR Jobs/run_fitterBias_toy_${i}.sh
done
