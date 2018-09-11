#######################################################################################
# Script for passing multiple root files and relevant arguments to MakeTuple executable
#######################################################################################
#/usr/bin/bash

# Check input args
if [[ $# -lt 6 ]]; then
    echo "Usage: ./MakeTuple.sh <year> <D0-mode> <data/MC>"\
    "<twoBody/fourBody/backgroundType> <polarity> <treename>"
    exit
fi

# Get input args
YEAR=$1
MODE=$2
TYPE=$3
CATEGORY=$4
MAG=$5
TREENAME=$6

# Make directory names
OUTPUT_DIR="${TYPE}/${CATEGORY}/${YEAR}_${MAG}"
# OUTPUT_DIR="15_${MAG}_old"
if [[ $TYPE == "MC" ]]; then
    INPUT_DIR="/data/lhcb/users/pullen/gangadir/job_output/${OUTPUT_DIR}"
else
    # INPUT_DIR="/data/lhcb/users/pullen/gangadir/job_output/data_preTISTOSfix/${CATEGORY}/${YEAR}_${MAG}"
    DIR=""
    if [[ $CATEGORY == "fourBody" ]]; then
        echo "fourBody"
        DIR="fourBody/"
    fi
    INPUT_DIR="/data/commonscratch/lhcb/pullen/gangadir/workspace/hpullen/LocalXML/${DIR}${YEAR}_${MAG}"
    if [[ $YEAR == 2012 || $YEAR == 2011 ]]; then
        INPUT_DIR="/data/lhcb/users/pullen/gangadir/job_output/data/${CATEGORY}/${YEAR}_${MAG}"
    fi
fi
echo "Looking for root files in ${INPUT_DIR}"

# cd and execute the script
cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/
./MakeTuple $OUTPUT_DIR $YEAR $MODE $TYPE $TREENAME $(find $INPUT_DIR/ -mindepth 3 -maxdepth 3 -name "*.root")
cd -
