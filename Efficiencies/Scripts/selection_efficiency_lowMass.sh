#!/usr/bin/env bash 
# Split into separate files
for MODE in {pi,gamma}_{010,100,001}; do
    FILE="../Selection/Results/lowMass/${MODE}.param"
    if [[ -f $FILE ]]; then rm $FILE; fi
    grep "$MODE" ../Selection/Results/selection_efficiency_lowMass.txt | \
        sed "s/^${MODE}_//" >> $FILE
done

# Average over 100 and 001 to create 101
for PART in gamma pi; do
    FILE="../Selection/Results/lowMass/${PART}_101.param"
    if [[ -f $FILE ]]; then rm $FILE; fi
    for YEAR in 201{2,6}_{up,down}; do
        VAL_100=$(awk "/$YEAR/{print \$2}" "../Selection/Results/lowMass/${PART}_100.param")
        VAL_001=$(awk "/$YEAR/{print \$2}" "../Selection/Results/lowMass/${PART}_001.param")
        ERR_100=$(awk "/$YEAR/{print \$3}" "../Selection/Results/lowMass/${PART}_100.param")
        ERR_001=$(awk "/$YEAR/{print \$3}" "../Selection/Results/lowMass/${PART}_001.param")
        echo "$YEAR $(bc -l <<< "($VAL_100 + $VAL_001)/2") "\
            "$(bc -l <<< "sqrt($ERR_100^2 + $ERR_001^2)")" >> $FILE
    done
done

# Average over years
VAR_LIST="-i"
for MODE in {gamma,pi}_{101,010}; do
    VAR_LIST="${VAR_LIST} $MODE ../Selection/Results/lowMass/${MODE}.param"
done
./AverageEfficiency ../Values/selection_lowMass.param $VAR_LIST
./AverageEfficiency ../Values/selection_lowMass_run1.param $VAR_LIST --run1
./AverageEfficiency ../Values/selection_lowMass_run2.param $VAR_LIST --run2

