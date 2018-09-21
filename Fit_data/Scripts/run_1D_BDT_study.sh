#!/usr/bin/env bash 

run_for_mode() {
    MODE=$1

    if [[ $MODE == "Kpi" || $MODE == "Kpipipi" ]]; then
        CUTS="-0.2 0.9 0.1"
    else 
        CUTS="0.4 0.95 0.05"
    fi

    sed "s/NAME/$MODE/;s/MODE/$MODE/;s/CUTS/$CUTS/" Templates/run_1D_BDT_study.sh\
        > Jobs/run_1D_BDT_study_${MODE}.sh
    qsub -q short -e Jobs/Outputs -o Jobs/Outputs Jobs/run_1D_BDT_study_${MODE}.sh
}

# Get mode if specified
if [[ $# == 1 ]]; then
    run_for_mode $1
else 
    for CURRENT_MODE in Kpi KK pipi Kpipipi pipipipi; do
        run_for_mode $CURRENT_MODE
    done
fi

