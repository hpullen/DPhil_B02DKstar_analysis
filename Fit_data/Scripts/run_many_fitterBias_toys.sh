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
TIME="11:59:59"
if grep "binned" <<< "$EXTRA_OPTS" > /dev/null; then
    TIME="01:59:59"
    EXTRA="/Binned/"
fi
if grep "combineRuns" <<< "$EXTRA_OPTS" > /dev/null; then
    EXTRA="${EXTRA}/CombinedRuns"
fi
if grep "high_stats" <<< "$EXTRA_OPTS" > /dev/null; then
    EXTRA="${EXTRA}/high_stats"
fi
if grep "split" <<< "$EXTRA_OPTS" > /dev/null; then
    EXTRA="${EXTRA}/split/"
fi
if grep "splitObs" <<< "$EXTRA_OPTS" > /dev/null; then
    EXTRA="${EXTRA}/SplitObs/"
fi

# Move old files to old folder
NEW_DIR=/data/lhcb/users/pullen/B02DKstar/toys/FitterBias/${EXTRA}/
mkdir -p $NEW_DIR
OLD_DIR=${NEW_DIR}/old
mkdir -p $OLD_DIR
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
    sed "s/TOYNUMBER/${i}/; s/EXTRAOPTS/${EXTRA_OPTS}/; s/11:59:59/${TIME}/" Templates/run_fitterBias_toy.sh > Jobs/run_fitterBias_toy_${i}.sh
    # OUTPUT=${NEW_DIR}/pulls_${i}.root
    # if [[ -e $OUTPUT ]]; then
        # echo $OUPUT already exists. Will not submit job $i
    # else
    qsub -lnodes=1:ppn=4 -e $LOGDIR -o $LOGDIR Jobs/run_fitterBias_toy_${i}.sh
    # fi
done
