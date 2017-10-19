#!/bin/sh

source /data/lhcb/sw/scripts/lbsetup-cvmfs-osagnostic.sh > /dev/null 2>&1
SetupProject Gaudi ROOT > /dev/null 2>&1

INPUTDIR=/data/lhcb/users/pullen/gangadir/job_output/
OUTPUTDIR=/data/lhcb/users/pullen/B02DKstar/
DATA_TYPE=$1

NEWDIR=${OUTPUTDIR}/${DATA_TYPE}
mkdir -p $NEWDIR

hadd -f ${NEWDIR}/Total.root ${INPUTDIR}/${DATA_TYPE}/*/output/*.root
