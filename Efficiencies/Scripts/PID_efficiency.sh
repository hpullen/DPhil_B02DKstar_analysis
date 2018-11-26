#!/usr/bin/env bash 
# Function to calculate PID efficiency for one config
calc_efficiencies() {

    # Input args
    FLAV=$1
    BINNING=$2
    EXTRA_OPT=""
    NAME="PID_efficiency"
    EXT=""
    if [[ $# == 3 ]]; then
        EXTRA_OPT=$3
    fi

    # Get particle name
    if [[ $FLAV == "combined" ]]; then
        PARTICLE=""
    else
        PARTICLE="_${FLAV}"
    fi

    # Get binning name
    if [[ $BINNING = "default" ]]; then
        BNAME=""
    else 
        BNAME="_alt"
    fi

    # Get modes to loop over
    MODES_SHARE=""
    if [[ $EXTRA_OPT == "" ]]; then
        MODES="Kpi KK pipi Kpipipi pipipipi"
        if [[ $FLAV == "combined" ]]; then
            MODES="Kpi KK pipi"
            MODES_SHARE="Kpipipi pipipipi"
        fi
    elif [[ $EXTRA_OPT == "--doubleSwap" ]]; then
        MODES="Kpi Kpipipi_low Kpipipi_high"
        NAME="${NAME}_doubleSwap"
        EXT="doubleSwap_"
    elif [[ $EXTRA_OPT == "--rho" ]]; then
        MODES="Kpi"
        NAME="${NAME}_rho"
        EXT="rho_"
    else
        echo "Unrecognised option: $EXTRA_OPT"
        return
    fi

    # Create input string
    INPUT_STR="-i"
    DIR="../PID/Results/"

    # Loop through modes with a single file
    for MODE in $MODES; do
        INPUT_STR="${INPUT_STR} $MODE ${DIR}/${EXT}${MODE}${PARTICLE}${BNAME}.param"
    done

    # Loop through modes with files to combine
    if [[ $FLAV == "combined" && $EXTRA_OPT != "--doubleSwap" ]]; then
        INPUT_STR="${INPUT_STR} -c"
        for MODE in $MODES_SHARE; do
            INPUT_STR="${INPUT_STR} $MODE ${DIR}/${EXT}${MODE}_B0${BNAME}.param ${DIR}/${EXT}${MODE}_B0bar${BNAME}.param"
        done
    fi
    echo $INPUT_STR

    # Calculate the average efficiency for each run
    ./AverageEfficiency ../Values/PID_raw/${NAME}${PARTICLE}${BNAME}.param $INPUT_STR 
    ./AverageEfficiency ../Values/PID_raw/${NAME}${PARTICLE}${BNAME}_run1.param $INPUT_STR --run1
    ./AverageEfficiency ../Values/PID_raw/${NAME}${PARTICLE}${BNAME}_run2.param $INPUT_STR --run2
}


# Get final PID efficiency with uncertainty
get_eff_with_err() {
    
    # Input args
    FLAV=$1
    if [[ $FLAV == "combined" ]]; then
        PARTICLE=""
    else
        PARTICLE="_${FLAV}"
    fi
    NAME="PID_efficiency"
    MODES="Kpi KK pipi Kpipipi pipipipi"
    if [[ $# == 2 ]]; then
        EXTRA_OPT=$2
        if [[ $EXTRA_OPT == "--rho" ]]; then
            NAME="${NAME}_rho"
            MODES="Kpi"
        elif [[ $EXTRA_OPT == "--doubleSwap" ]]; then
            NAME="${NAME}_doubleSwap"
            MODES="Kpi Kpipipi_low Kpipipi_high"
        else 
            echo "Unknown option: $EXTRA_OPT"
            return
        fi
    fi


    # Loop through runs
    for RUN in "" "_run1" "_run2"; do

        # File names
        DIR="../Values/"
        OUTFILE="${DIR}/${NAME}${PARTICLE}${RUN}.param"
        INFILE="${DIR}/PID_raw/${NAME}${PARTICLE}${RUN}.param"
        ALTFILE="${DIR}/PID_raw/${NAME}${PARTICLE}_alt${RUN}.param"
        if [[ -f $OUTFILE ]]; then rm $OUTFILE; fi

        # Get values and difference
        for MODE in $MODES; do
            VAL=$(awk "/^$MODE\ /{print \$2}" $INFILE)
            ALT_VAL=$(awk "/^$MODE\ /{print \$2}" $ALTFILE)
            ERR=$(bc -l <<< "$VAL - $ALT_VAL" | sed 's/-/0/')
            echo "$MODE $VAL $ERR" >> $OUTFILE
        done
    done
}


# Process each category
# Signal
for FLAV in "combined" "B0" "B0bar"; do
    calc_efficiencies $FLAV default
    calc_efficiencies $FLAV alt
    get_eff_with_err $FLAV
done

# Double mis-ID
calc_efficiencies combined default --doubleSwap
calc_efficiencies combined alt --doubleSwap
get_eff_with_err combined --doubleSwap


# Rho
calc_efficiencies combined default --rho
calc_efficiencies combined alt --rho
get_eff_with_err combined --rho
