#!/usr/bin/env bash 

# Script to apply PIDGen resampling to B0->Drho0 MC samples
# Polarity
YEAR=$1
MAG=$2
MAG_SHORT=$(awk '{print tolower(substr($0, 4))}' <<< $MAG)

# Arguments
INPUT="/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/${YEAR}_${MAG_SHORT}/Kpi_selected.root"
TREE="DecayTree"
OUTPUT="/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/${YEAR}_${MAG_SHORT}/Kpi_selected_resampled.root"
PIDVAR="KstarK_PIDK"
PTVAR="KstarK_PT"
ETAVAR="KstarK_ETA"
TRACKSVAR="LOKI_nTracks"
if [[ $YEAR == "2012" ]]; then
    CONFIG="pi_CombDLLK"
else 
    CONFIG="pi_CombDLLK_Brunel"
fi
DATASET=${MAG}_${YEAR}
SCALE="1.15" # Reweights nTracks to match data

# Run
python $PIDPERFSCRIPTSROOT/scripts/python/PIDGenUser/PIDGen.py -i $INPUT\
    -t $TREE -o $OUTPUT -p $PIDVAR -m $PTVAR -e $ETAVAR -n $TRACKSVAR -d $DATASET\
    -c $CONFIG --ntrscale $SCALE
