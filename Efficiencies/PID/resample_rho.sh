#!/usr/bin/env bash 

# Script to apply PIDGen resampling to B0->Drho0 MC samples
# Polarity
MAG=$1
MAG_SHORT=$(awk '{print tolower(substr($0, 4))}' <<< $MAG)

# Arguments
INPUT=/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/2016_${MAG_SHORT}/Kpi_selected.root
TREE=DecayTree
OUTPUT=/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/2016_${MAG_SHORT}/Kpi_selected_resampled.root
PIDVAR=KstarK_PIDK
PTVAR=KstarK_PT
ETAVAR=KstarK_ETA
TRACKSVAR=nTracks
CONFIG=K_CombDLLK_Brunel
DATASET=${MAG}_2016
SCALE="1.15"

# Run
python $PIDPERFSCRIPTSROOT/scripts/python/PIDGenUser/PIDGen.py -i $INPUT\
    -t $TREE -o $OUTPUT -p $PIDVAR -m $PTVAR -e $ETAVAR -n $TRACKSVAR -d $DATASET\
    -c $CONFIG --ntrscale $SCALE
