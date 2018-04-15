#!/usr/bin/env bash 


# Function to run a job
function run_BDT_job {

    # Location of template
    TEMPLATE=Jobs/template.sh
    ODIR=Jobs/Outputs
    EDIR=Jobs/Errors

    # Get cuts from input args
    ID=$1
    CUT_KPI=$2
    CUT_KK=$3
    CUT_PIPI=$4
    CUT_KPIPIPI=$5
    CUT_PIPIPIPI=$6

    # Make job file
    JOBFILE=Jobs/BDT_study_${ID}_Kpi_${CUT_KPI}_KK_${CUT_KK}_pipi_${CUT_PIPI}_Kpipipi_${CUT_KPIPIPI}_pipipipi_${CUT_PIPIPIPI}.sh
    sed "s/ID/${ID}/; s/CUT_KPI/${CUT_KPI}/; s/CUT_KK/${CUT_KK}/; s/CUT_PIPI/${CUT_PIPI}/; s/CUT_KPIPIPI/${CUT_KPIPIPI}/; s/CUT_PIPIPIPI/${CUT_PIPIPIPI}/;"\
        $TEMPLATE > $JOBFILE
    qsub $JOBFILE -e $EDIR -o $ODIR
}

# Loop through ID (4 jobs per study)
for ID in $(seq 0 1 9); do

    for CUT_KPI in $(seq 0.0 0.1 0.9); do

        # Kpi vs KK study
        for CUT_KK in $(seq 0.0 0.1 0.9); do
            run_BDT_job $ID $CUT_KPI $CUT_KK 0.5 0.5 0.5
        done

        # Kpi vs pipi study
        for CUT_PIPI in $(seq 0.0 0.1 0.9); do
            run_BDT_job $ID $CUT_KPI 0.5 $CUT_PIPI 0.5 0.5
        done

    done

    # Kpipipi vs pipipipi study
    for CUT_KPIPIPI in $(seq 0.0 0.1 0.9); do
        for CUT_PIPIPIPI in $(seq 0.0 0.1 0.9); do
            run_BDT_job $ID 0.5 0.5 0.5 $CUT_KPIPIPI $CUT_PIPIPIPI
        done
    done


done

