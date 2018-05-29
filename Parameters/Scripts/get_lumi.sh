#!/usr/bin/env bash 

# Output file
# OUTFILE=../lumi_calculated.param
OUTFILE=../lumi_calculated.param
if [[ -f $OUTFILE ]]; then rm $OUTFILE; fi

# Calculate luminosity for each polarity and year
for YEAR in 2011 2012 2015 2016; do
    for MAG in up down; do
        BASEDIR=$GANGADIR/job_output/data/twoBody/${YEAR}_${MAG}/
        LUMI_SUM=0
        SQ_ERR_SUM=0
        for LOG in $BASEDIR/*/output/*.log; do
            LUMI=$(grep "Integrated luminosity" $LOG | head -1 | awk '{print $5}')
            ERR=$(grep "Integrated luminosity" $LOG | head -1 | awk '{print sqrt($7)}')
            LUMI_SUM=$(echo "$LUMI_SUM + $LUMI" | bc -l)
            SQ_ERR_SUM=$(echo "$SQ_ERR_SUM + $ERR" | bc -l)
        done
        LUMI_ERR=$(echo "sqrt($SQ_ERR_SUM)" | bc -l)
        echo "$YEAR $MAG: $LUMI_SUM +/- $LUMI_ERR"
        echo "${YEAR}_${MAG} $LUMI_SUM $LUMI_ERR" >> $OUTFILE
    done 
done

