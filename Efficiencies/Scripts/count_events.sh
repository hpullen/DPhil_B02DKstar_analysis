#!/usr/bin/env bash
OUTPUT=/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Values/n_bookkeeping.txt
rm $OUTPUT
for mode in Kpi KK pipi; do
    for year in 2011 2012 2015 2016; do
        for mag in up down; do
            if [[ $year == "2011" && $mode != "Kpi" ]]; then
            else 
                SUM=0
                JOBDIR="$GANGADIR/job_output/MC/twoBody/${mode}/${year}_${mag}/"
                for file in ${JOBDIR}/*/output/summary.xml; do
                    ADD=$(grep "Tuple_${mode}/Event" $file | grep -o "[0-9]\+")
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
        for file in ${JOBDIR}/*/output/summary.xml; do
            ADD=$(grep "Tuple_K3pi/Event" $file | grep -o "[0-9][0-9]\+")
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
    for file in ${JOBDIR}/*/output/summary.xml; do
        ADD=$(grep "Tuple_4pi/Event" $file | grep -o "[0-9][0-9]\+")
        SUM=$((${SUM} + ${ADD}))
    done
    echo Sum for $mode $year $mag is $SUM
    echo $mode $year $mag $SUM >> $OUTPUT
done

LOWMASS_OUPUT="/home/pullen/analysis/B02DKstar/Efficiencies/Values/n_bookkeeping_lowMass.txt"
for mag in up down; do
    for particle in gamma pi; do
        for helicity in 010 100 001; do
            SUM=0
            year=2016
            JOBDIR="$GANGADIR/job_output/MC/backgrounds/lowMass/${particle}/${helicity}/${year}_${mag}/"
            for file in ${JOBDIR}/*/output/summary.xml; do
                ADD=$(grep "Tuple_Kpi/Event" $file | grep -o "[0-9][0-9]\+")
                SUM=$((${SUM} + ${ADD}))
            done
            echo Low mass sum for $particle $helicity $mag is $SUM
            echo $particle $helicity $mag $SUM >> "$LOWMASS_OUPUT"
        done
    done
done
