for MAG in MagUp MagDown; do
    for PARTICLE in B0 B0bar; do 
        for MODE in Kpi Kpipipi; do
            for YEAR in 2012 2016; do
                sh PID_efficiency_doubleSwap.sh $MODE $YEAR $MAG $PARTICLE
            done
        done
        sh PID_efficiency_doubleSwap.sh Kpi 2011 $MAG $PARTICLE
        sh PID_efficiency_doubleSwap.sh Kpi 2015 $MAG $PARTICLE
    done 
done

