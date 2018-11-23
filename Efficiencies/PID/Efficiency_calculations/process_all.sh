#!/usr/bin/env bash 

# Script to get all efficiency values for all modes

# Function to process for one mode
process_mode() {

    # Get settings
    MODE=$1
    FLAV=$2
    ALT=$3
    echo "Processing $MODE $FLAV $ALT"

    # Get name of output file
    if [[ $FLAV == "combined" ]]; then
        PARTICLE=""
    else 
        PARTICLE=$FLAV
    fi
    OUTFILE="../Results/${MODE}${PARTICLE}.param"
    if [[ $ALT == "--alt" ]]; then
        OUTFILE="../Results/${MODE}${PARTICLE}_alt.param"
    fi
    if [[ -f $OUTFILE ]]; then
        rm $OUTFILE
    fi

    # Process each year and polarity
    for YEAR in 2011 2012 2015 2016; do
        for MAG in up down; do

            # Check for PIDCalib results file
            PID_DIR="/data/lhcb/users/pullen/B02DKstar/PIDCalib/Results/"
            if [[ $ALT == "--alt" ]]; then
                PID_DIR="${PID_DIR}/Alternative/"
            fi
            PID_FILE="${PID_DIR}/${YEAR}_${MAG}/${MODE}${PARTICLE}.root"
            if [[ -f $PID_FILE ]]; then

                # Get mean and uncertainty
                root -b -q "get_efficiency.C(\"$PID_FILE\")" > temp
                MEAN=$(grep "Mean:" temp | sed 's/Mean:\ //')
                ERROR=$(grep "Uncertainty:" temp | sed 's/Uncertainty:\ //')
                echo "$MODE $PARTICLE $YEAR $MAG: $MEAN +\\- $ERROR"
                rm temp

                # Write to output file
                echo "${YEAR}_${MAG} $MEAN $ERROR" >> $OUTFILE

            fi

        done
    done
}


# Get option
OPT=$1
if [[ $# == 0 || $OPT == "--signal" ]]; then
    MODES="Kpi KK pipi Kpipipi pipipipi"
elif [[ $OPT == "--doubleSwap" ]]; then
    MODES="doubleSwap_Kpi doubleSwap_Kpipipi"
elif [[ $OPT == "--rho" ]]; then
    MODES="rho_Kpi"
fi

# Loop through modes
for MODE in $MODES; do
    for FLAV in "combined" "_B0" "_B0bar"; do
        process_mode $MODE $FLAV ""
        process_mode $MODE $FLAV "--alt"
    done
done
