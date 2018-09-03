for MAG in MagUp MagDown; do
    for PARTICLE in B0 B0bar; do
        sh PID_efficiency_split.sh 2012 $MAG Kpipipi $PARTICLE 
        sh PID_efficiency_split.sh 2016 $MAG Kpipipi $PARTICLE 
        sh PID_efficiency_split.sh 2016 $MAG pipipipi $PARTICLE 
    done
done
