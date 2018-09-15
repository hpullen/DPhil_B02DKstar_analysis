#!/usr/bin/env bash 

run_for_mode() {
    MODE=$1

    sed "s/NAME/$MODE/;s/MODE/$MODE/" Templates/run_1D_BDT_study.sh\
        > Jobs/run_1D_BDT_study_${MODE}.sh
    qsub -q veryshort -e Jobs/Outputs -o Jobs/Outputs Jobs/run_1D_BDT_study_${MODE}.sh
}

# Get mode if specified
if [[ $# == 1 ]]; then
    run_for_mode $1
else 
    for CURRENT_MODE in Kpi KK pipi Kpipipi pipipipi; do
        run_for_mode $CURRENT_MODE
    done
fi

