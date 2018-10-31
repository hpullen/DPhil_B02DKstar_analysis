#!/usr/bin/env bash 

# Script to get all efficiency values for all modes

# Function to process for one mode
process_mode() {

    # Get settings
    MODE=$1
    PARTICLE=$2

    # Get name of output file
    OUTFILE="../Results/${MODE}${PARTICLE}_alt.param"
    if [[ -f $OUTFILE ]]; then
        rm $OUTFILE
    fi

    # Process each year and polarity
    for YEAR in 2011 2012 2015 2016; do
        for MAG in up down; do

            # Check for PIDCalib results file
            PID_FILE="/data/lhcb/users/pullen/B02DKstar/PIDCalib/Results/${YEAR}_${MAG}/${MODE}${PARTICLE}_withPIDeffs_alt.root"
            if [[ -f $PID_FILE ]]; then

                # Get mean and uncertainty
                root -b -q "get_efficiency.C(\"$PID_FILE\")" > temp
                MEAN=$(grep "Mean:" temp | sed 's/Mean: \(.*\)/\1/')
                ERROR=$(grep "Uncertainty:" temp | sed 's/Uncertainty: \(.*\)/\1/')
                echo "$MODE $PARTICLE $YEAR $MAG: $MEAN +\\- $ERROR"
                rm temp

                # Write to output file
                echo "${YEAR}_${MAG} $MEAN $ERROR" >> $OUTFILE

            fi

        done
    done
}


# Loop through modes
for MODE in Kpi KK pipi Kpipipi pipipipi; do
    for PARTICLE in "" "_B0" "_B0bar"; do
        process_mode $MODE $PARTICLE
    done
done
