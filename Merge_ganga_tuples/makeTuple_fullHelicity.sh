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
OUTPUT_DIR="${TYPE}/${CATEGORY}/${YEAR}_${MAG}/"
INPUT_DIR="/data/lhcb/users/pullen/gangadir/job_output/${OUTPUT_DIR}"

# cd and execute the script
cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/
./MakeTuple "$OUTPUT_DIR/Full_helicity" $YEAR $MODE $TYPE $TREENAME $(find $INPUT_DIR/ -mindepth 3 -maxdepth 3 -name "*.root")
cd -
