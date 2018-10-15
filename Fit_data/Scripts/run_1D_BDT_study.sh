#!/usr/bin/env bash 
run_for_mode() {
    MODE=$1

    if [[ $MODE == "Kpi" || $MODE == "Kpipipi" ]]; then
        CUTS="-0.2 0.9 0.1"
    else 
        CUTS="0.4 0.95 0.05"
    fi

    DIR="/data/lhcb/users/pullen/B02DKstar/BDT_studies/"
    sed "s/NAME/$MODE/;s/MODE/$MODE/;s/CUTS/$CUTS/" Templates/run_1D_BDT_study.sh\
        > $DIR/scripts/data/run_1D_BDT_study_${MODE}.sh
    qsub -lnodes=1:ppn=3 -q short -e "$DIR/job_outputs/data" -o "$DIR/job_outputs/data" $DIR/scripts/data/run_1D_BDT_study_${MODE}.sh
}

# Get mode if specified
if [[ $# == 1 ]]; then
    run_for_mode $1
else 
    # for CURRENT_MODE in Kpi KK pipi Kpipipi pipipipi; do
    for CURRENT_MODE in KK pipi pipipipi; do
        run_for_mode $CURRENT_MODE
    done
fi

