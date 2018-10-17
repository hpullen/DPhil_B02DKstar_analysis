#!/usr/bin/env bash 

# Get mode
MODE=$1
if [[ $# == 0 ]]; then
    echo "Usage: ./run_MC_BDT_toys <MODE>"
    exit
fi

# Job output dir
DIR="/data/lhcb/users/pullen/B02DKstar/BDT_studies/MC_based/"

# Make sure mode directory exists
MODE_DIR=$DIR/toys/$MODE
if [[ ! -d $MODE_DIR ]]; then
    mkdir $MODE_DIR
fi

# Loop through cuts
CUTS=$(seq -0.5 0.1 0.0)
for CUT in $CUTS; do

    # Check cut directory exists
    CUT_DIR=$MODE_DIR/$CUT
    if [[ ! -d $CUT_DIR ]]; then
        mkdir $CUT_DIR
    fi

    # Run 50 x 20 = 1000 jobs for each
    for ID in $(seq 1 1 20); do

        OUTFILE=$CUT_DIR/toy_${ID}.root
        JOBFILE=$DIR/scripts/BDT_toy_${MODE}_${CUT}_${ID}.sh
        sed "s:OUTFILE:${OUTFILE}:; s:${MODE}_cut:${CUT}:" \
            Templates/run_MC_BDT_toy.sh | sed 's/[A-Za-z]\+_cut/0.5/g' > $JOBFILE
        qsub $JOBFILE -o "$DIR/outputs" -e "$DIR/outputs"

    done

done

