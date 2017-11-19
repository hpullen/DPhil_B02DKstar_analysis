#/usr/bin/bash
OUTPUT=/home/pullen/analysis/B02DKstar/Efficiencies/Values/n_bookkeeping.txt
rm $OUTPUT
for mode in Kpi KK pipi; do
    for year in 2015 2016; do
        for mag in up down; do
            SUM=0
            JOBDIR="$GANGADIR/job_output/MC/twoBody/${mode}/${year}_${mag}/"
            for file in ${JOBDIR}/*/output/summary.xml; do
                ADD=$(grep "Tuple_${mode}/Event" $file | grep -o "[0-9]\+")
                SUM=$((${SUM} + ${ADD}))
            done
            echo Sum for $mode $year $mag is $SUM
            echo $mode $year $mag $SUM >> $OUTPUT
        done
    done
done

for mag in up down; do
    SUM=0
    mode=Kpipipi
    year=2016
    JOBDIR="$GANGADIR/job_output/MC/fourBody/${mode}/${year}_${mag}/"
    for file in ${JOBDIR}/*/output/summary.xml; do
        ADD=$(grep "Tuple_K3pi/Event" $file | grep -o "[0-9][0-9]\+")
        SUM=$((${SUM} + ${ADD}))
    done
    echo Sum for $mode $year $mag is $SUM
    echo $mode $year $mag $SUM >> $OUTPUT
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
