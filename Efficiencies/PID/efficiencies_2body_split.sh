for MAG in MagUp MagDown; do
    for PARTICLE in B0 B0bar; do
        for MODE in Kpi KK pipi; do
            for YEAR in 2012 2015 2016; do
                sh PID_efficiency_split.sh $YEAR $MAG $MODE $PARTICLE 
            done
        done
        sh PID_efficiency_split.sh 2011 $MAG Kpi $PARTICLE 
    done
done
