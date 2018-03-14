#!/usr/bin/env bash

# Loop through and submit
COUNTER=1
for i in $(seq 0 0.1 0.9); do
    sed "s/NAME/KK_Kpi_${COUNTER}/;s/KK_CUT/${i}/;s/KPI_MIN/0.0/;s/KPI_MAX/0.4/" Templates/run_BDT_study_KK_Kpi.sh > Jobs/run_BDT_study_KK_Kpi_${COUNTER}.sh
    qsub -q veryshort -e Jobs/Outputs/ -o Jobs/Outputs/ Jobs/run_BDT_study_KK_Kpi_${COUNTER}.sh
    COUNTER=$((COUNTER + 1))
    sed "s/NAME/KK_Kpi_${COUNTER}/;s/KK_CUT/${i}/;s/KPI_MIN/0.5/;s/KPI_MAX/0.9/" Templates/run_BDT_study_KK_Kpi.sh > Jobs/run_BDT_study_KK_Kpi_${COUNTER}.sh
    qsub -q veryshort -e Jobs/Outputs/ -o Jobs/Outputs/ Jobs/run_BDT_study_KK_Kpi_${COUNTER}.sh
    COUNTER=$((COUNTER + 1))
done
