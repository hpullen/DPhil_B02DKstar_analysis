LOGDIR="/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/batch_jobs/logs/"
for MODE in Kpi piK KK pipi Kpipipi piKpipi pipipipi; do 
    JOB=". /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/makeTuple.sh 2018 $MODE data twoBody down Tuple_$MODE/DecayTree"
    qsub -q short -o $LOGDIR -e $LOGDIR <<< "$JOB" 
    JOB=". /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/makeTuple.sh 2018 $MODE data twoBody up Tuple_$MODE/DecayTree"
    qsub -q short -o $LOGDIR -e $LOGDIR <<< "$JOB" 
    # JOB=". /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/makeTuple.sh 2018 $MODE data twoBody down Tuple_$MODE/DecayTree"
    # qsub -q short -o $LOGDIR -e $LOGDIR <<< "$JOB"
done
