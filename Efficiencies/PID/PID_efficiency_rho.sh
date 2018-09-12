#!/usr/bin/env bash 
# Calculate PID efficiencies for B->Drho MC

# Input args
YEAR=$1
MAG=$2

# Get stripping name
if [[ $YEAR == "2012" ]]; then
    STRIP="20"
elif [[ $YEAR == "2016" ]]; then
    STRIP="Turbo16"
fi

# Get option for ETA variable name
if [[ $YEAR == "2012" ]]; then
    ETA_OPT='-y LOKI_ETA'
else 
    ETA_OPT='-y ETA'
fi

# Lowercase name of polarity
MAG_SHORT=$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')

# D0 PID cuts for mode
D0_PID1='[D0K, K, DLLK > 1]'
D0_PID2='[D0Pi, Pi, DLLK < -1]'

# Other options
DATA_ROOT=/data/lhcb/users/pullen/B02DKstar/
EFFDIR=/home/pullen/analysis/B02DKstar/Efficiencies/PID/
INFILE=$DATA_ROOT/MC/backgrounds/rho/${YEAR}_$MAG_SHORT/Kpi_selected.root
TREENAME="DecayTree"
OUTFILE=$DATA_ROOT/PIDCalib/Results/${YEAR}_$MAG_SHORT/rho_withPIDeffs.root
LOGFILE=$EFFDIR/logs/${YEAR}_$MAG_SHORT/rho.txt
Kstar_PID1='[KstarK, Pi, DLLK > 5]'
Kstar_PID2='[KstarPi, Pi, DLLK < -1]'
if [[ $YEAR == "2012" ]]; then
    P_NAME='P'
    ETA_NAME='ETA'
else 
    P_NAME='Brunel_P'
    ETA_NAME='Brunel_ETA'
fi

# Run command
cd $DATA_ROOT/PIDCalib/PerfHists/
python /home/pullen/analysis/B02DKstar/Efficiencies/PID/UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/PerformMultiTrackCalib.py -z "" -Z "" $STRIP $MAG $INFILE $TREENAME $OUTFILE -X $P_NAME -Y $ETA_NAME $ETA_OPT "$D0_PID1" "$D0_PID2" "$Kstar_PID1" "$Kstar_PID2" | tee $LOGFILE
