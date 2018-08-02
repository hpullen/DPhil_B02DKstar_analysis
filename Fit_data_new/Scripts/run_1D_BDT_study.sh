#!/usr/bin/env bash 

# Get mode
MODE=$1
sed "s/NAME/$MODE/;s/MODE/$MODE/" Templates/run_1D_BDT_study.sh\
    > Jobs/run_1D_BDT_study_${MODE}.sh
qsub -q short -e Jobs/Outputs -o Jobs/Outputs Jobs/run_1D_BDT_study_${MODE}.sh
