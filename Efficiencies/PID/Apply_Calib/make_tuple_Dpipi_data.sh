#!/usr/bin/env bash 
# General script to create a calibrated tuple based on PerfHists

# Setup
YEAR="2016"
STRIP="Turbo16"

# D0 PID cuts 
D0_PID1="[D0K, K, DLLK > 1]"
D0_PID2='[D0Pi, Pi, DLLK < -1]'

# Kstar PID cuts (mis-ID pion)
Kstar_PID1='[KstarK, Pi, DLLK > 5]'
Kstar_PID2='[KstarPi, Pi, DLLK < -1]'

# Other options
DATA_ROOT="/data/lhcb/users/pullen/B02DKstar/"
TREENAME="DecayTreekpi"
SCRIPTFILE="/home/pullen/analysis/B02DKstar/Efficiencies/PID/UraniaDev_v8r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/PerformMultiTrackCalib.py"
EFFDIR="/home/pullen/analysis/B02DKstar/Efficiencies/PID/"
OUTDIR=$DATA_ROOT/PIDCalib/Results/
echo Input file: $INFILE

# Move to perfhist directory
cd $DATA_ROOT/PIDCalib/PerfHists/

for MAG in down ; do

    # Options depending on polarity
    INFILE=$DATA_ROOT/Mark_tuple/Kpi_${MAG}_withETA.root
    OUTNAME=$OUTDIR/2016_${MAG}/Dpipi_data.root
    LOGFILE=$EFFDIR/logs/Dpipi_data_2016_${MAG}.log
    if [[ $MAG == "up" ]]; then
        MAG_LONG="MagUp"
    else 
        MAG_LONG="MagDown"
    fi
    echo Output file: $OUTNAME

    # Run command (extra arg for K3pi due to third PID cut)
    python $SCRIPTFILE -x "P" -y "ETA" -z "nTracks" -X "Brunel_P" -Y "Brunel_ETA" -Z "nTracks_Brunel" \
        $STRIP $MAG_LONG $INFILE $TREENAME $OUTNAME "$D0_PID1" "$D0_PID2" "$Kstar_PID1" "$Kstar_PID2" | tee $LOGFILE

done
