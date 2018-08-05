#!/usr/bin/env bash 

# Get mode
MODE=$1

# Make sure mode directory exists
MODE_DIR=/data/lhcb/users/pullen/B02DKstar/BDT_studies/$MODE
if [[ ! -d $MODE_DIR ]]; then
    mkdir $MODE_DIR
fi

# Loop through cuts
for CUT in $(seq 0.1 0.1 0.9); do

    # Check cut directory exists
    CUT_DIR=$MODE_DIR/$CUT
    if [[ ! -d $CUT_DIR ]]; then
        mkdir $CUT_DIR
    fi

    # Run 50 jobs for each
    for ID in $(seq 1 1 50); do

        OUTFILE=$CUT_DIR/toy_${ID}.root
        JOBFILE=Jobs/BDT_toy_${MODE}_${CUT}_${ID}.sh
        sed "s:OUTFILE:${OUTFILE}:; s:${MODE}_cut:${CUT}:" \
            Templates/run_1D_BDT_toy.sh | sed 's/[A-Za-z]\+_cut/0.5/g' > $JOBFILE
        qsub $JOBFILE -o Jobs/Output -e Jobs/Output

    done

done

