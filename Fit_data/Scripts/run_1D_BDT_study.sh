#!/usr/bin/env bash 
run_for_mode() {
    MODE=$1
    HIGH=False
    if [[ $2 == "--high" ]]; then
        HIGH=True
    fi

    if [[ $MODE == "Kpi" || $MODE == "Kpipipi" ]]; then
        if [[ $HIGH == True ]]; then
            CUTS="0.92 0.98 0.03"
        else 
            CUTS="-0.5 0.9 0.1"
        fi
    else 
        if [[ $HIGH == True ]]; then
            CUTS="0.91 0.99 0.02"
        else 
            CUTS="0.0 0.9 0.1"
        fi
    fi

    DIR="/data/lhcb/users/pullen/B02DKstar/BDT_studies/"
    NAME=$MODE
    if [[ $HIGH == True ]]; then
        NAME="${NAME}_high"
    fi
    sed "s/NAME/$NAME/;s/MODE/${MODE}/;s/CUTS/$CUTS/" Templates/run_1D_BDT_study.sh\
        > $DIR/scripts/data/run_1D_BDT_study_${NAME}.sh
    qsub -lnodes=1:ppn=3 -q short -e "$DIR/job_outputs/data" -o "$DIR/job_outputs/data" $DIR/scripts/data/run_1D_BDT_study_${NAME}.sh
}

# Get mode if specified
if [[ $# == 1 ]]; then
    run_for_mode $1
    run_for_mode $1 --high
else 
    # for CURRENT_MODE in Kpi KK pipi Kpipipi pipipipi; do
    for CURRENT_MODE in KK pipi pipipipi; do
        run_for_mode $CURRENT_MODE
    done
fi

