#!/usr/bin/env bash 
# Average over 100 and 001 to create 101
for PART in gamma pi; do
    FILE="../Generator/lowMass/${PART}_101.param"
    if [[ -f $FILE ]]; then rm $FILE; fi
    for YEAR in 2016_{up,down}; do
        VAL_100=$(awk "/$YEAR/{print \$2}" "../Generator/lowMass/${PART}_100.param")
        VAL_001=$(awk "/$YEAR/{print \$2}" "../Generator/lowMass/${PART}_001.param")
        ERR_100=$(awk "/$YEAR/{print \$3}" "../Generator/lowMass/${PART}_100.param")
        ERR_001=$(awk "/$YEAR/{print \$3}" "../Generator/lowMass/${PART}_001.param")
        echo "$YEAR $(bc -l <<< "($VAL_100 + $VAL_001)/2") "\
            "$(bc -l <<< "sqrt($ERR_100^2 + $ERR_001^2)")" >> $FILE
    done
done

# Average over all
VAR_LIST="-i"
for MODE in {gamma,pi}_{101,010}; do
    VAR_LIST="${VAR_LIST} $MODE ../Generator/lowMass/${MODE}.param"
done
./AverageEfficiency ../Values/acceptance_lowMass.param $VAR_LIST 
