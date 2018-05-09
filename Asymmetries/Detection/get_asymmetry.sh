#!/usr/bin/env bash

# Get input args
YEAR=$1
MAG=$2
PARENT=$3 # D0/Kpi

# Make args for Kpi asymmetry
POLARITY=mag${MAG}
INPUT=/data/lhcb/users/pullen/B02DKstar/data/twoBody/${YEAR}_${MAG}/Kpi_selected_withWeights.root
K_PX=${PARENT}K_PX
K_PY=${PARENT}K_PY
K_PZ=${PARENT}K_PZ
PI_PX=${PARENT}Pi_PX
PI_PY=${PARENT}Pi_PY
PI_PZ=${PARENT}Pi_PZ
PIDPI=-1
if [[ $PARENT == "D0" ]]; then
    PIDK=1
else 
    PIDK=5
fi
if [[ $YEAR == "2016" ]]; then
    PIDARGS="--k_pid=${PIDK} --pi_pid=${PIDPI}"
else
    PIDARGS=""
fi

# Check directory exists and is empty
OUTDIR=/home/pullen/analysis/B02DKstar/Asymmetries/Detection/Results/$PARENT/${YEAR}_${MAG}
if [[ -f $OUTDIR ]]; then
    rm -r $OUTDIR/*
else 
    mkdir -p $OUTDIR
fi

# Run
cd AsymmetryTools/projects/KPiTwoTrackAsymmetry/
COMMAND="python main.py --year $YEAR --polarity $POLARITY --output_directory $OUTDIR\
    -f $INPUT --input_tuple_name DecayTree --k_px_alias $K_PX --k_py_alias $K_PY\
    --k_pz_alias $K_PZ --pi_px_alias $PI_PX --pi_py_alias $PI_PY --pi_pz_alias $PI_PZ\
    $PIDARGS --sweight_var sw_signal"
echo $COMMAND
exec $COMMAND

# Remove any ROOT files
rm $(find /home/pullen/analysis/B02DKstar/Asymmetries/Detection/Results/ -name "*.root")
