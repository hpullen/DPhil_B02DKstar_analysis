#!/usr/bin/env bash 
# Calculate PID efficiencies for two body MC

# Input args
YEAR=$1
MAG=$2
MODE=$3

# Get stripping name
if [[ $YEAR == "2011" ]]; then
    STRIP="20r1"
elif [[ $YEAR == "2012" ]]; then
    STRIP="20"
elif [[ $YEAR == "2015" ]]; then
    STRIP="Turbo15"
elif [[ $YEAR == "2016" ]]; then
    STRIP="Turbo16"
fi

# Get option for ETA variable name
if [[ $YEAR == "2011" || $YEAR == "2012" ]]; then
    ETA_OPT='-y LOKI_ETA'
else 
    ETA_OPT='-y ETA'
fi

# Lowercase name of polarity
MAG_SHORT=$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')

# D0 PID cuts for mode
if [[ $MODE == "Kpi" ]]; then
    D0_PID1='[D0K, K, DLLK > 1.0]'
    D0_PID2='[D0Pi, Pi, DLLK < -1.0]'
elif [[ $MODE == "KK" ]]; then
    D0_PID1='[D0Kplus, K, DLLK > 1.0]' 
    D0_PID2='[D0Kminus, K, DLLK > 1.0]'
elif [[ $MODE == "pipi" ]]; then
    D0_PID1='[D0PiPlus, Pi, DLLK < -1.0]'
    D0_PID2='[D0PiMinus, Pi, DLLK < -1.0]'
fi

# Other options
DATA_ROOT=/data/lhcb/users/pullen/B02DKstar/
BOD="twoBody"
EFFDIR=/home/pullen/analysis/B02DKstar/Efficiencies/PID/
INFILE=$DATA_ROOT/MC/$BOD/$MODE/${YEAR}_$MAG_SHORT/${MODE}_selected.root
TREENAME="DecayTree"
OUTFILE=$DATA_ROOT/PIDCalib/Results/${YEAR}_$MAG_SHORT/${MODE}_withPIDeffs.root
LOGFILE=$EFFDIR/logs/${YEAR}_$MAG_SHORT/${MODE}.txt
Kstar_PID1='[KstarK, K, DLLK > 5.0]'
Kstar_PID2='[KstarPi, Pi, DLLK < -1.0]'

# Run command
cd $DATA_ROOT/PIDCalib/PerfHists/
python /home/pullen/analysis/B02DKstar/Efficiencies/PID/UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/PerformMultiTrackCalib.py -z "" -Z "" $STRIP $MAG $INFILE $TREENAME $OUTFILE $Z_OPTS $ETA_OPT "$D0_PID1" "$D0_PID2" "$Kstar_PID1" "$Kstar_PID2" | tee $LOGFILE
