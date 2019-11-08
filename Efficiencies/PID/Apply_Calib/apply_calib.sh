#!/usr/bin/env bash 
# General script to create a calibrated tuple based on PerfHists

# Get input args
if [[ $# < 4 ]]; then
    echo "Usage: ./apply_calib.sh <YEAR> <MagUp/MagDown> <MODE> <B0/B0bar/combined> (<doubleSwap/doubleSwap_Kstar/rho/lowMass/Bs_lowMass/rho_lowMass>)"
    exit -1
fi
YEAR=$1
MAG=$2
MODE=$3
IN_MODE=$MODE
if [[ $MODE == "Kpipipi_low" || $MODE == "Kpipipi_high" ]]; then
    IN_MODE="Kpipipi"
fi
PARTICLE=$4
EXTRA_OPT=""
if [[ $# == 5 ]]; then
    if [[ $IN_MODE != "Kpipipi" && $IN_MODE != "Kpi" ]]; then
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
if [[ $EXTRA_OPT != "rho" && $EXTRA_OPT != "rho_lowMass" && $EXTRA_OPT != "lowMass" && $EXTRA_OPT != "Bs_lowMass" ]]; then
    if [[ $IN_MODE == "Kpipipi" || $IN_MODE == "pipipipi" ]]; then
        LOC=fourBody/$IN_MODE
        if [[ $PARTICLE == "combined" ]] && [[ $MODE == "Kpipipi" || $MODE == "pipipipi" ]]; then
            echo Error: 4-body PID calculation must be split!
            exit
        fi
    elif [[ $MODE == "Kpi" || $MODE == "KK" || $MODE == "pipi" ]]; then
        LOC=twoBody/$MODE
    fi
else 
    if [[ $EXTRA_OPT == "rho" ]]; then
        if [[ $MODE == "Kpi" ]]; then
            LOC=backgrounds/rho
        else 
            LOC=backgrounds/rho_Kpipipi
        fi
    else 
        LOC=backgrounds/${EXTRA_OPT}
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
if [[ $EXTRA_OPT == "" || $EXTRA_OPT == "rho" || $EXTRA_OPT == "doubleSwap_Kstar" 
    || $EXTRA_OPT == "rho_lowMass" || $EXTRA_OPT == "lowMass" || $EXTRA_OPT == "Bs_lowMass" ]]; then
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
    if [[ $MODE == "Kpi" ]]; then
        D0_PID2='[D0Pi, Pi, DLLK > 1]'
    elif [[ $MODE == "Kpipipi_low" ]]; then
        D0_PID2='[D0Pi_low, Pi, DLLK > 1]'
        D0_PID3='[D0Pi_high, Pi, DLLK < -1]'
    elif [[ $MODE == "Kpipipi_high" ]]; then
        D0_PID2='[D0Pi_high, Pi, DLLK > 1]'
        D0_PID3='[D0Pi_low, Pi, DLLK < -1]'
    else 
        echo "Mode for doubleSwap should be Kpi, Kpipipi_low or Kpipipi_high"
        exit -1;
    fi
fi

# Kstar PID cuts
if [[ $EXTRA_OPT == "rho"  || $EXTRA_OPT == "doubleSwap_Kstar" || 
    $EXTRA_OPT == "rho_lowMass" ]]; then
    Kstar_PID1='[KstarK, Pi, DLLK > 5]'
else
    Kstar_PID1='[KstarK, K, DLLK > 5]'
fi
if [[ $EXTRA_OPT == "doubleSwap_Kstar" ]]; then
    Kstar_PID2='[KstarPi, K, DLLK < -1]'
else 
    Kstar_PID2='[KstarPi, Pi, DLLK < -1]'
fi

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
    INFILE=$DATA_ROOT/MC/$LOC/${YEAR}_$MAG_SHORT/${IN_MODE}_selected_${PARTICLE}.root
    OUTNAME=${YEAR}_$MAG_SHORT/${NAME}_${PARTICLE}.root
    LOGFILE=$EFFDIR/logs/${YEAR}_$MAG_SHORT/${NAME}_${PARTICLE}.txt
else
    INFILE=$DATA_ROOT/MC/$LOC/${YEAR}_$MAG_SHORT/${IN_MODE}_selected.root
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
    if [[ $EXTRA_OPT != "rho_lowMass" && $EXTRA_OPT != "lowMass" && $EXTRA_OPT != "Bs_lowMass" ]]; then
        if [[ $IN_MODE == "Kpipipi" ]]; then
            python $SCRIPTFILE -z "" -Z "" $STRIP $MAG $INFILE $TREENAME "$OUTDIR/${DIR}/${OUTNAME}" \
                $Z_OPTS $ETA_OPT "$D0_PID1" "$D0_PID2" "$D0_PID3" "$Kstar_PID1" "$Kstar_PID2" \
                -W "weight" $PERFHIST_OPTS | tee $LOGFILE
        else
            python $SCRIPTFILE -z "" -Z "" $STRIP $MAG $INFILE $TREENAME "$OUTDIR/${DIR}/${OUTNAME}" \
                $Z_OPTS $ETA_OPT "$D0_PID1" "$D0_PID2" "$Kstar_PID1" "$Kstar_PID2" \
                -W "weight" $PERFHIST_OPTS | tee $LOGFILE
        fi
    else 
        for PARTICLE in gamma pi; do
            for HELICITY in 010 100 001; do
                INFILE=$DATA_ROOT/MC/$LOC/$PARTICLE/$HELICITY/${YEAR}_$MAG_SHORT/${IN_MODE}_selected.root
                OUTNAME=${YEAR}_$MAG_SHORT/${NAME}_${PARTICLE}_${HELICITY}.root
                echo $SCRIPTFILE -z "" -Z "" $STRIP $MAG $INFILE $TREENAME "$OUTDIR/${DIR}/${OUTNAME}" \
                    $Z_OPTS $ETA_OPT "$D0_PID1" "$D0_PID2" "$Kstar_PID1" "$Kstar_PID2" \
                    -W "weight" $PERFHIST_OPTS | tee $LOGFILE
                python $SCRIPTFILE -z "" -Z "" $STRIP $MAG $INFILE $TREENAME "$OUTDIR/${DIR}/${OUTNAME}" \
                    $Z_OPTS $ETA_OPT "$D0_PID1" "$D0_PID2" "$Kstar_PID1" "$Kstar_PID2" \
                    -W "weight" $PERFHIST_OPTS | tee $LOGFILE
            done
        done
    fi

done
