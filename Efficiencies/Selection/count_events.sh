#!/usr/bin/env bash
OUTPUT=/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Results/n_bookkeeping.txt
if [[ -f $OUTPUT ]]; then
    rm $OUTPUT
fi
for mode in Kpi KK pipi; do
    for year in 2011 2012 2015 2016; do
        for mag in up down; do
            if [[ $year == "2011" && $mode != "Kpi" ]]; then
                echo "Ignoring 2011 for mode != Kpi"
            else
                SUM=0
                JOBDIR="$GANGADIR/job_output/MC/twoBody/${mode}/${year}_${mag}/"
                for file in ${JOBDIR}/*/output/*.log; do
                    ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9]\+")
                    SUM=$((${SUM} + ${ADD}))
                done
                echo Sum for $mode $year $mag is $SUM
                echo $mode $year $mag $SUM >> $OUTPUT
            fi
        done
    done
done

for mag in up down; do
    for year in 2012 2016; do
        SUM=0
        mode=Kpipipi
        JOBDIR="$GANGADIR/job_output/MC/fourBody/${mode}/${year}_${mag}/"
        for file in ${JOBDIR}/*/output/*.log; do
            ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9][0-9]\+")
            SUM=$((${SUM} + ${ADD}))
        done
        echo Sum for $mode $year $mag is $SUM
        echo $mode $year $mag $SUM >> $OUTPUT
    done
done

for mag in up down; do
    SUM=0
    mode=pipipipi
    year=2016
    JOBDIR="$GANGADIR/job_output/MC/fourBody/${mode}/${year}_${mag}/"
    for file in ${JOBDIR}/*/output/*.log; do
        ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9][0-9]\+")
        SUM=$((${SUM} + ${ADD}))
    done
    echo Sum for $mode $year $mag is $SUM
    echo $mode $year $mag $SUM >> $OUTPUT
done

LOWMASS_OUTPUT="/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Results/n_bookkeeping_lowMass.txt"
if [[ -f $LOWMASS_OUTPUT ]]; then
    rm $LOWMASS_OUTPUT
fi
for mag in up down; do
    for particle in gamma pi; do
        for helicity in 010 100 001; do
            for year in 2012 2016; do
                SUM=0
                JOBDIR="$GANGADIR/job_output/MC/backgrounds/lowMass/${particle}/${helicity}/${year}_${mag}/"
                for file in ${JOBDIR}/*/output/*.log; do
                    ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9][0-9]\+")
                    SUM=$((${SUM} + ${ADD}))
                done
                echo Low mass sum for $particle $helicity $mag is $SUM
                echo $particle $helicity $year $mag $SUM >> "$LOWMASS_OUTPUT"
            done
        done
    done
done
