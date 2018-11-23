LOGDIR="/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/batch_jobs/logs/"
for YEAR in 2011 2012 2015 2016; do
    for MAG in up down; do
        for MODE in Kpi piK KK pipi; do
            JOB=". /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/makeTuple.sh $YEAR $MODE data twoBody $MAG Tuple_$MODE/DecayTree"
            qsub -q short -o $LOGDIR -e $LOGDIR <<< "$JOB" 
        done
        for MODE in Kpipipi piKpipi pipipipi; do
            JOB=". /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/makeTuple.sh $YEAR $MODE data fourBody $MAG Tuple_$MODE/DecayTree"
            qsub -q short -o $LOGDIR -e $LOGDIR <<< "$JOB" 
        done
    done
done

