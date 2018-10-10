#!/usr/bin/env bash 

# Get mode
MODE=$1
if [[ $# == 0 ]]; then
    echo "Usage: ./run_1D_BDT_toys.sh <MODE>"
    exit
fi

# Input/output directory
DIR="/data/lhcb/users/pullen/B02DKstar/BDT_studies/"

# Make sure mode directory exists
MODE_DIR=$DIR/toys/$MODE
if [[ ! -d $MODE_DIR ]]; then
    mkdir $MODE_DIR
fi

# Loop through cuts
if [[ $MODE == "Kpi" || $MODE == "Kpipipi" ]]; then
    # CUTS=$(seq -0.2 0.1 0.9)
    CUTS=$(seq -0.5 0.1 -0.3)
else 
    CUTS=$(seq 0.4 0.05 0.95)
    # CUTS=0.95
fi
for CUT in $CUTS; do

    # Check cut directory exists
    CUT_DIR=$MODE_DIR/$CUT
    if [[ ! -d $CUT_DIR ]]; then
        mkdir $CUT_DIR
    fi

    # Run 50 x 20 = 1000 jobs for each
    for ID in $(seq 1 1 20); do

        OUTFILE=$CUT_DIR/toy_${ID}.root
        JOBFILE=$DIR/scripts/toys/BDT_toy_${MODE}_${CUT}_${ID}.sh
        sed "s:OUTFILE:${OUTFILE}:; s:${MODE}_cut:${CUT}:" \
            Templates/run_1D_BDT_toy.sh | sed 's/[A-Za-z]\+_cut/0.5/g' > $JOBFILE
        qsub $JOBFILE -o "$DIR/job_outputs/toys" -e "$DIR/job_outputs/toys"

    done

done

