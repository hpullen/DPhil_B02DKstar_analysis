#!/usr/bin/env bash 
# Average over 100 and 001 to create 101
for PART in {Bs_lowMass,lowMass}_Kpi_{gamma,pi}; do
    FILE="../PID/Results/lowMass/${PART}_101.param"
    if [[ -f $FILE ]]; then rm $FILE; fi
    for YEAR in 201{2,6}_{up,down}; do
        VAL_100=$(awk "/$YEAR/{print \$2}" "../PID/Results/lowMass/${PART}_100.param")
        VAL_001=$(awk "/$YEAR/{print \$2}" "../PID/Results/lowMass/${PART}_001.param")
        ERR_100=$(awk "/$YEAR/{print \$3}" "../PID/Results/lowMass/${PART}_100.param")
        ERR_001=$(awk "/$YEAR/{print \$3}" "../PID/Results/lowMass/${PART}_001.param")
        echo "$YEAR $(bc -l <<< "($VAL_100 + $VAL_001)/2") "\
            "$(bc -l <<< "sqrt($ERR_100^2 + $ERR_001^2)")" >> $FILE
    done
done

# Average over years
VAR_LIST="-i"
for MODE in {Bs_lowMass,lowMass}_Kpi_{gamma,pi}_{101,010}; do
    VAR_LIST="${VAR_LIST} $MODE ../PID/Results/lowMass/${MODE}.param"
done
./AverageEfficiency ../Values/PID_lowMass.param $VAR_LIST
./AverageEfficiency ../Values/PID_lowMass_run1.param $VAR_LIST --run1
./AverageEfficiency ../Values/PID_lowMass_run2.param $VAR_LIST --run2
./AverageEfficiency ../Values/PID_lowMass_allYears.param $VAR_LIST --allYears

# Rho low mass
# VAR_LIST="-i"
# for MODE in {gamma,pi}_{101,010}; do
    # VAR_LIST="${VAR_LIST} $MODE ../Selection/Results/rho_lowMass/${MODE}.param"
# done
# ./AverageEfficiency ../Values/selection_rho_lowMass.param $VAR_LIST
