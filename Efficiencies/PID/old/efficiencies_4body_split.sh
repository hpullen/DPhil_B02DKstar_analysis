if [[ $1 == "--alt" ]]; then
    ALT=True
fi
for MAG in MagUp MagDown; do
    for PARTICLE in B0 B0bar; do
        if [[ $ALT == True ]]; then
            sh PID_efficiency_alt.sh 2012 $MAG Kpipipi $PARTICLE 
            sh PID_efficiency_alt.sh 2016 $MAG Kpipipi $PARTICLE 
            sh PID_efficiency_alt.sh 2016 $MAG pipipipi $PARTICLE 
        else 
            sh PID_efficiency_split.sh 2012 $MAG Kpipipi $PARTICLE 
            sh PID_efficiency_split.sh 2016 $MAG Kpipipi $PARTICLE 
            sh PID_efficiency_split.sh 2016 $MAG pipipipi $PARTICLE 
        fi
    done
done
