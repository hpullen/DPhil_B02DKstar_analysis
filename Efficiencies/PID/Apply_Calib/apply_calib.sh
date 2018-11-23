#!/usr/bin/env bash 
# General script to create a calibrated tuple based on PerfHists

# Get input args
if [[ $# < 4 ]]; then
    echo "Usage: ./apply_calib.sh <YEAR> <MagUp/MagDown> <MODE> <B0/B0bar/combined> (<doubleSwap/rho>)"
    exit -1
fi
YEAR=$1
MAG=$2
MODE=$3
PARTICLE=$4
EXTRA_OPT=""
if [[ $# == 5 ]]; then
    if [[ $MODE != "Kpipipi" && $MODE != "Kpi" ]]; then
        echo "Error: extra options only available for Kpi and Kpipipi."
        exit -1
    fi
    EXTRA_OPT=$5
fi

# Print some info
echo "YEAR: $YEAR"
echo "MAG: $MAG"
echo "MODE: $MODE"
echo "FLAVOUR: $PARTICLE"

# Get stripping name
if [[ $YEAR == "2011" ]]; then
    STRIP="20r1"
elif [[ $YEAR == "2012" ]]; then
    STRIP="20"
elif [[ $YEAR == "2015" ]]; then
    STRIP="Turbo15"
elif [[ $YEAR == "2016" ]]; then
    STRIP="Turbo16"
else 
    echo "! Unrecognised year: $YEAR"
    exit -1
fi

# Sub directory location of MC files
if [[ $EXTRA_OPT != "rho" ]]; then
    if [[ $MODE == "Kpipipi" || $MODE == "pipipipi" ]]; then
        LOC=fourBody/$MODE
        if [[ $PARTICLE == "combined" ]]; then
            echo Error: 4-body PID calculation must be split!
            exit
        fi
    elif [[ $MODE == "Kpi" || $MODE == "KK" || $MODE == "pipi" ]]; then
        LOC=twoBody/$MODE
    fi
else 
    if [[ $MODE == "Kpi" ]]; then
        LOC=backgrounds/rho
    else 
        LOC=backgrounds/rho_Kpipipi
    fi
fi

# Get option for ETA variable name
if [[ $YEAR == "2011" || $YEAR == "2012" ]]; then
    ETA_OPT='-y LOKI_ETA'
    PERFHIST_OPTS='-X P -Y ETA'
else
    ETA_OPT='-y ETA'
    PERFHIST_OPTS='-X Brunel_P -Y Brunel_ETA'
fi

# Lowercase name of polarity
if [[ $MAG != "MagDown" && $MAG != "MagUp" ]]; then
    echo "Polarity should be MagDown or MagUp."
    exit -1
fi
MAG_SHORT=$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')

# D0 PID cuts for mode/type
if [[ $EXTRA_OPT == "" || $EXTRA_OPT == "rho" ]]; then
    # Two-body modes: cuts at +/- 1
    if [[ $MODE == "Kpi" ]]; then
        D0_PID1="[D0K, K, DLLK > 1]"
        D0_PID2='[D0Pi, Pi, DLLK < -1]'
    elif [[ $MODE == "KK" ]]; then
        D0_PID1="[D0Kplus, K, DLLK > 1]"
        D0_PID2="[D0Kminus, K, DLLK > 1]"
    elif [[ $MODE == "pipi" ]]; then
        D0_PID1='[D0PiPlus, Pi, DLLK < -1]'
        D0_PID2='[D0PiMinus, Pi, DLLK < -1]'

    # Four-body modes: cut depends on flavour
    elif [[ $MODE == "Kpipipi" ]]; then
        D0_PID1="[D0K, K, DLLK > 1]"
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

# Double mis-ID: swap cuts
elif [[ $EXTRA_OPT == "doubleSwap" ]]; then
    D0_PID1='[D0K, K, DLLK < -1]'
    D0_PID2='[D0Pi, Pi, DLLK > 1]'
    if [[ $MODE == "Kpipipi" ]]; then
        if [[ $PARTICLE == "B0" ]]; then
            D0_PID3='[D0PiMinus, Pi, DLLK < -1]'
        else
            D0_PID3='[D0PiPlus, Pi, DLLK < -1]'
        fi
    fi
fi

# Kstar PID cuts
if [[ $EXTRA_OPT != "rho" ]]; then
    Kstar_PID1='[KstarK, K, DLLK > 5]'
else
    Kstar_PID1='[KstarK, Pi, DLLK > 5]'
fi
Kstar_PID2='[KstarPi, Pi, DLLK < -1]'

# Name for output files
if [[ $EXTRA_OPT == "" ]]; then
    NAME=$MODE
else
    NAME="${EXTRA_OPT}_${MODE}"
fi

# Other options
TREENAME="DecayTree"
SCRIPTFILE="/home/pullen/analysis/B02DKstar/Efficiencies/PID/UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/PerformMultiTrackCalib.py"
DATA_ROOT="/data/lhcb/users/pullen/B02DKstar/"
EFFDIR="/home/pullen/analysis/B02DKstar/Efficiencies/PID/"
OUTDIR=$DATA_ROOT/PIDCalib/Results/
if [[ $PARTICLE != "combined" ]]; then
    INFILE=$DATA_ROOT/MC/$LOC/${YEAR}_$MAG_SHORT/${MODE}_selected_${PARTICLE}.root
    OUTNAME=${YEAR}_$MAG_SHORT/${NAME}_${PARTICLE}.root
    LOGFILE=$EFFDIR/logs/${YEAR}_$MAG_SHORT/${NAME}_${PARTICLE}.txt
else
    INFILE=$DATA_ROOT/MC/$LOC/${YEAR}_$MAG_SHORT/${MODE}_selected.root
    OUTNAME=${YEAR}_$MAG_SHORT/${NAME}.root
    LOGFILE=$EFFDIR/logs/${YEAR}_$MAG_SHORT/${NAME}.txt
fi
echo Input file: $INFILE
echo Output file: ${OUTDIR}/${OUTNAME}

# Make normal and alternative PID tuples
for DIR in "" "Alternative"; do

    # Move to perfhist directory
    cd $DATA_ROOT/PIDCalib/PerfHists/$DIR

    # Run command (extra arg for K3pi due to third PID cut)
    if [[ $MODE == "Kpipipi" ]]; then
        python $SCRIPTFILE -z "" -Z "" $STRIP $MAG $INFILE $TREENAME "$OUTDIR/${DIR}/${OUTNAME}" \
            $Z_OPTS $ETA_OPT "$D0_PID1" "$D0_PID2" "$D0_PID3" "$Kstar_PID1" "$Kstar_PID2" \
            $PERFHIST_OPTS | tee $LOGFILE
    else
        python $SCRIPTFILE -z "" -Z "" $STRIP $MAG $INFILE $TREENAME "$OUTDIR/${DIR}/${OUTNAME}" \
            $Z_OPTS $ETA_OPT "$D0_PID1" "$D0_PID2" "$Kstar_PID1" "$Kstar_PID2" \
            $PERFHIST_OPTS | tee $LOGFILE
    fi

done
