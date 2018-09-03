for MAG in MagUp MagDown; do
    for MODE in Kpi KK pipi; do
        for YEAR in 2012 2015 2016; do
            sh PID_efficiency_split.sh $YEAR $MAG $MODE 
        done
    done
    sh PID_efficiency_split.sh 2011 $MAG Kpi 
done
