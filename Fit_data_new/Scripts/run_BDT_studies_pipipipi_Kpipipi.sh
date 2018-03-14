#!/usr/bin/env bash

# Loop through and submit
COUNTER=1
for i in $(seq 0 0.1 0.9); do
    sed "s/NAME/pipipipi_Kpipipi_${COUNTER}/;s/PIPIPIPI_CUT/${i}/;s/KPIPIPI_MIN/0.0/;s/KPIPIPI_MAX/0.4/" Templates/run_BDT_study_pipipipi_Kpipipi.sh > Jobs/run_BDT_study_pipipipi_Kpipipi_${COUNTER}.sh
    qsub -q veryshort -e Jobs/Outputs/ -o Jobs/Outputs/ Jobs/run_BDT_study_pipipipi_Kpipipi_${COUNTER}.sh
    COUNTER=$((COUNTER + 1))
    sed "s/NAME/pipipipi_Kpipipi_${COUNTER}/;s/PIPIPIPI_CUT/${i}/;s/KPIPIPI_MIN/0.5/;s/KPIPIPI_MAX/0.9/" Templates/run_BDT_study_pipipipi_Kpipipi.sh > Jobs/run_BDT_study_pipipipi_Kpipipi_${COUNTER}.sh
    qsub -q veryshort -e Jobs/Outputs/ -o Jobs/Outputs/ Jobs/run_BDT_study_pipipipi_Kpipipi_${COUNTER}.sh
    COUNTER=$((COUNTER + 1))
done
