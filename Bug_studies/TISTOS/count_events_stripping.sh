#!/usr/bin/env bash 

MODE=$1
YEAR=$2
MAG=$3

if [[ $MODE == "Kpipipi" || $MODE == "pipipipi" || $MODE == "piKpipi" ]];
then
    BOD="fourBody"
else
    BOD="twoBody"
fi

./CountEvents $MODE $(find /data/lhcb/users/pullen/gangadir/job_output/data/${BOD}/${YEAR}_${MAG} -name "*.root")
