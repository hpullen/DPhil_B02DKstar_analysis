#!/usr/bin/env bash 

# Script to get PID efficiency for rho sample


process_one() {

    # Get particle
    PARTICLE=$1

    # Get name of output file
    OUTFILE="../Results/rho${MODE}${PARTICLE}.param"
    if [[ -f $OUTFILE ]]; then
        rm $OUTFILE
    fi

    # Process each year and polarity
    for YEAR in 2012 2016; do
        for MAG in up down; do

            # Check for PIDCalib results file
            PID_FILE="/data/lhcb/users/pullen/B02DKstar/PIDCalib/Results/${YEAR}_${MAG}/rho${MODE}${PARTICLE}_withPIDeffs.root"
            if [[ -f $PID_FILE ]]; then

                # Get mean and uncertainty
                root -b -q "get_efficiency.C(\"$PID_FILE\")" > temp
                MEAN=$(grep "Mean:" temp | sed 's/Mean: \(.*\)/\1/')
                ERROR=$(grep "Uncertainty:" temp | sed 's/Uncertainty: \(.*\)/\1/')
                echo "$YEAR $MAG: $MEAN +\\- $ERROR"
                rm temp

                # Write to output file
                echo "${YEAR}_${MAG} $MEAN $ERROR" >> $OUTFILE

            fi

        done
    done

}



# Get mode
MODE=$1
if [[ $MODE == "_Kpipipi" ]]; then
    process_one "_B0"
    process_one "_B0bar"
else
    process_one ""
fi
