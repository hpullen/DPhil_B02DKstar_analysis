#!/usr/bin/env bash

# Number of jobs to run
if [[ $# == 0 ]]; then
    echo "Usage: ./run_significance_study.sh <type>"
    exit 1
fi
N_JOBS=$1
TYPE=$2
shift && shift
EXTRAS=$*

# Check output folder exists
DIR="/data/lhcb/users/pullen/B02DKstar/systematics/"
echo Extras: $EXTRAS
DIR_NAME=""
if grep 'combineRuns' <<< $EXTRAS > /dev/null; then
    DIR_NAME="/CombinedRuns"
fi
if grep 'splitObs' <<< $EXTRAS > /dev/null; then
    DIR_NAME="${DIR_NAME}/SplitObs"
fi
if grep 'unbinned' <<< $EXTRAS > /dev/null; then
    DIR_NAME="${DIR_NAME}/Unbinned"
fi
DIR_NAME=${DIR_NAME}/${TYPE}
if [[ ! -d "$DIR/$DIR_NAME" ]]; then
    echo Making dir $DIR/$DIR_NAME
    mkdir -p $DIR/$DIR_NAME
fi

# Time to run
if grep "unbinned" <<< $EXTRAS > /dev/null; then
    TIME="11:59:59"
else 
    TIME="01:59:59"
fi

# Loop through and submit
for i in $(seq 1 1 $N_JOBS); do
    sed "s/NUMBER/${i}/; s/EXTRAOPTS/${TYPE} ${EXTRAS}/; s/TIME/${TIME}/" Templates/run_systematics.sh > $DIR/Jobs/run_systematics_${TYPE}_${i}.sh
    OUTPUT=${DIR}/${DIR_NAME}/sys_${i}.root
    if [[ -e $OUTPUT ]]; then
        echo $OUPUT already exists. Will not submit job $i
    else 
        qsub -lnodes=1:ppn=4 -e $DIR/Outputs/ -o $DIR/Outputs/ $DIR/Jobs/run_systematics_${TYPE}_${i}.sh
    fi
done
