#!/usr/bin/env bash 
# Calculate PID efficiencies for two body MC

# Input args
YEAR=$1
MAG=$2
MODE=$3
if [[ $# == 4 ]]; then
    SPLIT=TRUE
    PARTICLE=$4
fi

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
if [[ $MODE == "Kpipipi" || $MODE == "pipipipi" ]]; then
    BOD=fourBody
    if [[ $SPLIT != "TRUE" ]]; then
        echo Error: 4-body PID calculation must be split!
        exit
    fi
else 
    BOD=twoBody
fi

# Get option for ETA variable name
if [[ $YEAR == "2011" || $YEAR == "2012" ]]; then
    ETA_OPT='-y LOKI_ETA'
    PERFHIST_OPTS='-X P -Y ETA'
    K_1_CUT='DLLK > 1.0'
    K_5_CUT='DLLK > 5.0'
else 
    ETA_OPT='-y ETA'
    PERFHIST_OPTS='-X Brunel_P -Y Brunel_ETA'
    K_1_CUT='DLLK > 1'
    K_5_CUT='DLLK > 5'
fi

# Lowercase name of polarity
MAG_SHORT=$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')

# D0 PID cuts for mode
if [[ $MODE == "Kpi" ]]; then
    D0_PID1="[D0K, K, $K_1_CUT]"
    D0_PID2='[D0Pi, Pi, DLLK < -1]'
elif [[ $MODE == "KK" ]]; then
    D0_PID1="[D0Kplus, K, $K_1_CUT]" 
    D0_PID2="[D0Kminus, K, $K_1_CUT]"
elif [[ $MODE == "pipi" ]]; then
    D0_PID1='[D0PiPlus, Pi, DLLK < -1]'
    D0_PID2='[D0PiMinus, Pi, DLLK < -1]'
elif [[ $MODE == "Kpipipi" ]]; then
    D0_PID1="[D0K, K, $K_1_CUT]"
    D0_PID2='[D0Pi, Pi, DLLK < -1]'
    if [[ $PARTICLE == "B0" ]]; then
        D0_PID3='[D0PiMinus, Pi, DLLK < -1]'
    else 
        D0_PID3='[D0PiPlus, Pi, DLLK < -1]'
    fi
elif [[ $MODE == "pipipipi" ]]; then
    if [[ $PARTICLE == "B0" ]]; then
        D0_PID1='[D0PiPlus1, Pi, DLLK < -1]'
        D0_PID2='[D0PiPlus2, Pi, DLLK < -1]'
    else 
        D0_PID1='[D0PiMinus1, Pi, DLLK < -1]'
        D0_PID2='[D0PiMinus2, Pi, DLLK < -1]'
    fi
fi

# Other options
DATA_ROOT=/data/lhcb/users/pullen/B02DKstar/
EFFDIR=/home/pullen/analysis/B02DKstar/Efficiencies/PID/
if [[ $SPLIT == "TRUE" ]]; then
    INFILE=$DATA_ROOT/MC/$BOD/$MODE/${YEAR}_$MAG_SHORT/${MODE}_selected_${PARTICLE}.root
    OUTFILE=$DATA_ROOT/PIDCalib/Results/${YEAR}_$MAG_SHORT/${MODE}_${PARTICLE}_withPIDeffs.root
    LOGFILE=$EFFDIR/logs/${YEAR}_$MAG_SHORT/${MODE}_${PARTICLE}.txt
else 
    INFILE=$DATA_ROOT/MC/$BOD/$MODE/${YEAR}_$MAG_SHORT/${MODE}_selected.root
    OUTFILE=$DATA_ROOT/PIDCalib/Results/${YEAR}_$MAG_SHORT/${MODE}_withPIDeffs.root
    LOGFILE=$EFFDIR/logs/${YEAR}_$MAG_SHORT/${MODE}.txt
fi
echo Will save results to logfile: $LOGFILE
TREENAME="DecayTree"
Kstar_PID1="[KstarK, K, $K_5_CUT]"
Kstar_PID2='[KstarPi, Pi, DLLK < -1]'

# Run command
cd $DATA_ROOT/PIDCalib/PerfHists/
if [[ $MODE == "Kpipipi" ]]; then
    python /home/pullen/analysis/B02DKstar/Efficiencies/PID/UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/PerformMultiTrackCalib.py -z "" -Z "" $STRIP $MAG $INFILE $TREENAME $OUTFILE $Z_OPTS $ETA_OPT "$D0_PID1" "$D0_PID2" "$D0_PID3" "$Kstar_PID1" "$Kstar_PID2" $PERFHIST_OPTS | tee $LOGFILE
else 
    python /home/pullen/analysis/B02DKstar/Efficiencies/PID/UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/PerformMultiTrackCalib.py -z "" -Z "" $STRIP $MAG $INFILE $TREENAME $OUTFILE $Z_OPTS $ETA_OPT "$D0_PID1" "$D0_PID2" "$Kstar_PID1" "$Kstar_PID2" $PERFHIST_OPTS | tee $LOGFILE
fi
