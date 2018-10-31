#!/usr/bin/env bash 

# Get mode and number of jobs
MODE=$1
N_JOBS=$2
if [[ $# == 0 ]]; then
    echo "Usage: ./run_many_significance_toys.sh <MODE> <NUMBER>"
    exit
fi

# Input/output directory
DIR="/data/lhcb/users/pullen/B02DKstar/toys/significance/"

# Make sure mode directory exists
MODE_DIR=$DIR/toys/$MODE
if [[ ! -d $MODE_DIR ]]; then
    mkdir $MODE_DIR
fi

# Run N jobs (10 toys per job)
for ID in $(seq 1 1 $N_JOBS); do

    OUTFILE=$MODE_DIR/toy_${ID}.root
    JOBFILE=$DIR/jobs/${MODE}_${ID}.sh
    sed "s:OUTFILE:${OUTFILE}:; s:MODE:${MODE}:" \
        Templates/run_significance_toy.sh > $JOBFILE
    qsub $JOBFILE -o "$DIR/outputs/" -e "$DIR/outputs/"

done
