#!/usr/bin/env bash 

# Script to combine selection and generator efficiencies for each mode
if [[ $1 == "--rho" ]]; then
    EXTRA="_rho"
elif [[ $1 == "--Bs" ]]; then
    EXTRA="_Bs"
else
    EXTRA=""
fi
for RUN in run1 run2; do 

    # Locations of acceptance and selection efficiency files
    ACC_FILE="../Values/acceptance_efficiency${EXTRA}_${RUN}.param"
    SEL_FILE="../Values/selection_efficiency${EXTRA}_${RUN}.param"

    # Make file and check it's empty
    OUTFILE="../Values/total_efficiency${EXTRA}_${RUN}.param"
    if [[ -f $OUTFILE ]]; then
        rm $OUTFILE
        touch $OUTFILE
    fi

    # Loop through modes
    for MODE in $(awk '{print $1}' $SEL_FILE); do

        # Skip 4pi Run 2
        if [[ $MODE == "pipipipi" && $RUN = "run1" ]]; then
            continue
        fi

        # Get values and uncertainties
        EFF_ACC=$(awk "/^$MODE\ /{print \$2}" $ACC_FILE)
        ERR_ACC=$(awk "/^$MODE\ /{print \$3}" $ACC_FILE)
        EFF_SEL=$(awk "/^$MODE\ /{print \$2}" $SEL_FILE)
        ERR_SEL=$(awk "/^$MODE\ /{print \$3}" $SEL_FILE)

        # Calculate totals
        EFF_TOT=$(bc -l <<< "$EFF_ACC * $EFF_SEL")
        ERR_TOT=$(bc -l <<< "$EFF_TOT * sqrt(($ERR_SEL/$EFF_SEL)^2 + ($ERR_ACC/$EFF_ACC)^2)")

        # Print to file
        echo "$MODE $EFF_TOT $ERR_TOT" >> $OUTFILE
    done
done
