#!/usr/bin/env bash 
# Calculate PID efficiencies for Lambda_b -> DpK MC

# Input args
YEAR=2016
MAG=$1

# Get stripping name
STRIP="Turbo16"

# Get option for ETA variable name
ETA_OPT='-y ETA'

# Lowercase name of polarity
MAG_SHORT=$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')

# D0 PID cuts for mode
D0_PID1='[D0K, K, DLLK > 1]'
D0_PID2='[D0Pi, Pi, DLLK < -1]'

# Other options
DATA_ROOT=/data/lhcb/users/pullen/B02DKstar/
EFFDIR=/home/pullen/analysis/B02DKstar/Efficiencies/PID/
INFILE=$DATA_ROOT/MC/backgrounds/Lambda_b/${YEAR}_$MAG_SHORT/Kpi_selected.root
TREENAME="DecayTree"
OUTFILE=$DATA_ROOT/PIDCalib/Results/${YEAR}_$MAG_SHORT/Lambda_K_withPIDeffs.root
LOGFILE=$EFFDIR/logs/${YEAR}_$MAG_SHORT/Lambda_K.txt
Kstar_PID1='[KstarK, Pi, DLLK > 5]'
Kstar_PID2='[KstarPi, Pi, DLLK < -1]'

# Variable names in calib sample
P_NAME='Brunel_P'
ETA_NAME='Brunel_ETA'

# Run command
cd $DATA_ROOT/PIDCalib/PerfHists/
python /home/pullen/analysis/B02DKstar/Efficiencies/PID/UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/PerformMultiTrackCalib.py -z "" -Z "" $STRIP $MAG $INFILE $TREENAME $OUTFILE -X $P_NAME -Y $ETA_NAME $ETA_OPT "$D0_PID1" "$D0_PID2" "$Kstar_PID1" "$Kstar_PID2" | tee $LOGFILE
