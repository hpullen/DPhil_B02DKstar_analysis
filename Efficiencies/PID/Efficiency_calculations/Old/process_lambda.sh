#!/usr/bin/env bash 

# Script to get PID efficiency for Lambda_b sample
# Get name of output file
OUTFILE="../Results/Lambda_K.param"
if [[ -f $OUTFILE ]]; then
    rm $OUTFILE
fi

# Process each year and polarity
for YEAR in 2016; do
    for MAG in up down; do

        # Check for PIDCalib results file
        PID_FILE="/data/lhcb/users/pullen/B02DKstar/PIDCalib/Results/${YEAR}_${MAG}/Lambda_K_withPIDeffs.root"
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

