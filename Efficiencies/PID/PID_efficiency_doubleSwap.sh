#!/usr/bin/env bash 
# Calculate PID efficiencies for two body MC

# Input args
MODE=$1
YEAR=$2
MAG=$3
PARTICLE=$4

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

# Two or four body
if [[ $MODE == "Kpi" ]]; then
    BOD=twoBody
else 
    BOD=fourBody
fi

# Get option for ETA variable name
if [[ $YEAR == "2011" || $YEAR == "2012" ]]; then
    ETA_OPT='-y LOKI_ETA'
    EXT=""
else 
    ETA_OPT='-y ETA'
    EXT="Brunel_"
fi

# Lowercase name of polarity
MAG_SHORT=$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')

# D0 PID cuts for mode
D0_PID1='[D0K, K, DLLK < -1.0]'
D0_PID2='[D0Pi, Pi, DLLK > 1.0]'
if [[ $MODE == "Kpipipi" ]]; then
    if [[ $PARTICLE == "B0" ]]; then
        D0_PID3='[D0PiMinus, Pi, DLLK > 1.0]'
    else 
        D0_PID3='[D0PiPlus, Pi, DLLK > 1.0]'
    fi
fi

# Other options
DATA_ROOT=/data/lhcb/users/pullen/B02DKstar/
EFFDIR=/home/pullen/analysis/B02DKstar/Efficiencies/PID/
INFILE=$DATA_ROOT/MC/$BOD/$MODE/${YEAR}_$MAG_SHORT/${MODE}_selected_${PARTICLE}.root
TREENAME="DecayTree"
OUTFILE=$DATA_ROOT/PIDCalib/Results/${YEAR}_$MAG_SHORT/${MODE}_${PARTICLE}_withPIDeffs_doubleSwap.root
LOGFILE=$EFFDIR/logs/${YEAR}_$MAG_SHORT/${MODE}_${PARTICLE}_doubleSwap.txt

# Run command
cd $DATA_ROOT/PIDCalib/PerfHists/swapped_hypothesis/
if [[ $MODE == "Kpipipi" ]]; then
    python /home/pullen/analysis/B02DKstar/Efficiencies/PID/UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/PerformMultiTrackCalib.py -z "" -Z "" $STRIP $MAG $INFILE $TREENAME $OUTFILE $ETA_OPT "$D0_PID1" "$D0_PID2" "$D0_PID3" -X "${EXT}P" -Y "${EXT}ETA" | tee $LOGFILE
else 
    python /home/pullen/analysis/B02DKstar/Efficiencies/PID/UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/PerformMultiTrackCalib.py -z "" -Z "" $STRIP $MAG $INFILE $TREENAME $OUTFILE $ETA_OPT "$D0_PID1" "$D0_PID2" -X "${EXT}P" -Y "${EXT}ETA" | tee $LOGFILE
fi
