#!/usr/bin/env bash 
# Get input args
YEAR=$1
MAG=$2
PARTICLE=$3

# Get data type
if [[ $YEAR == "2011" ]]; then
    DATA_TYPE="20r1"
elif [[ $YEAR == "2012" ]]; then
    DATA_TYPE="20"
elif [[ $YEAR == "2015" ]]; then
    DATA_TYPE="Turbo15"
elif [[ $YEAR == "2016" ]]; then
    DATA_TYPE="Turbo16"
fi

# Get cut
if [[ $PARTICLE == "K" ]]; then
    CUT='DLLK < -1.0'
elif [[ $PARTICLE == "Pi" ]]; then
    CUT='DLLK > 1.0'
fi

# Get variable names 
if [[ "$YEAR" == "2011" || "$YEAR" == "2012" ]]; then
    X_VAR="P"
    Y_VAR="ETA"
else
    X_VAR="Brunel_P"
    Y_VAR="Brunel_ETA"
fi

# Run command
python UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/MakePerfHistsRunRange.py "$DATA_TYPE" "$MAG" "$PARTICLE" "$CUT" "$X_VAR" "$Y_VAR" ""
