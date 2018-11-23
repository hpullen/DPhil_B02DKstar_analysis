for MAG in MagUp MagDown; do
    for PARTICLE in "" B0 B0bar; do
        for YEAR in 2011 2012 2015 2016; do
                sh PID_efficiency_split.sh $YEAR $MAG Bs $PARTICLE 
        done
    done
done
