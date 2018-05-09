#!/usr/bin/env bash 

BOD=$1
YEAR=$2
MAG=$3

DIR=/data/lhcb/users/pullen/gangadir/job_output/data/${BOD}/${YEAR}_${MAG}
CURRENT=$(pwd)
cd $DIR
grep -l "memory-resident" */output/*.log | tee $CURRENT/bad_files/${BOD}_${YEAR}_${MAG}.txt
