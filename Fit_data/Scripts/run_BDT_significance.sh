#!/usr/bin/env bash 

# Get mode
MODE=$1
if [[ $# == 0 ]]; then
    echo "Usage: ./run_1D_BDT_toys.sh <MODE>"
    exit
fi

# Input/output directory
DIR="/data/lhcb/users/pullen/B02DKstar/toys/significance/"

# Make sure mode directory exists
MODE_DIR=$DIR/toys/BDT/$MODE
if [[ ! -d $MODE_DIR ]]; then
    mkdir $MODE_DIR
fi

# Loop through cuts
CUTS=$(seq -0.5 0.1 0.9)
for CUT in $CUTS; do

    # Check cut directory exists
    CUT_DIR=$MODE_DIR/$CUT
    if [[ ! -d $CUT_DIR ]]; then
        mkdir $CUT_DIR
    fi

    # Run 10 x 20 = 2000 jobs for each
    for ID in $(seq 21 1 100); do

        OUTFILE=$CUT_DIR/toy_${ID}.root
        JOBFILE=$DIR/jobs/BDT/BDT_toy_${MODE}_${CUT}_${ID}.sh
        sed "s:OUTFILE:${OUTFILE}:; s:MODE:${MODE}:" \
            Templates/run_BDT_significance.sh | sed "s/CUT/${CUT}/g" > $JOBFILE
        qsub $JOBFILE -o "$DIR/outputs/BDT" -e "$DIR/outputs/BDT"

    done

done

