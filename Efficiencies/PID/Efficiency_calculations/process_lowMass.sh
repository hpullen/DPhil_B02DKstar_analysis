#!/usr/bin/env bash 

# Script to get all efficiency values for all modes

# Function to process for one mode
process_mode() {

    # Get settings
    MISSED=$1
    HEL=$2
    PARENT=$3
    FLAV=$4
    ALT=$5
    MODE=${PARENT}_Kpi_${MISSED}_${HEL}
    echo "Processing $MODE $FLAV $ALT"

    # Get name of output file
    if [[ $FLAV == "combined" ]]; then
        PARTICLE=""
    else 
        PARTICLE=$FLAV
    fi
    OUTFILE="../Results/lowMass/${MODE}${PARTICLE}.param"
    if [[ $ALT == "--alt" ]]; then
        OUTFILE="../Results/lowMass/${MODE}${PARTICLE}_alt.param"
    fi
    if [[ -f $OUTFILE ]]; then
        rm $OUTFILE
    fi

    # Process each year and polarity
    for YEAR in 2012 2016; do
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

# Loop through types
PARTICLES="gamma pi"
HELS="010 100 001"
for MISSED in $PARTICLES; do
    for HEL in $HELS; do
        for PARENT in "Bs_lowMass" "lowMass"; do
            process_mode $MISSED $HEL $PARENT combined "" 
            process_mode $MISSED $HEL $PARENT combined "--alt" 
        done
    done
done
