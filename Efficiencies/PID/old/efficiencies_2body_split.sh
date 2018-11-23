ALT=False
if [[ $1 == "--alt" ]]; then
    ALT=True
fi
for MAG in MagUp MagDown; do
    for PARTICLE in B0 B0bar; do
        for MODE in Kpi KK pipi; do
            for YEAR in 2012 2015 2016; do
                if [[ $ALT == True ]]; then
                    sh PID_efficiency_alt.sh $YEAR $MAG $MODE $PARTICLE 
                else
                    sh PID_efficiency_split.sh $YEAR $MAG $MODE $PARTICLE 
                fi
            done
        done
            if [[ $ALT == True ]]; then
                sh PID_efficiency_alt.sh 2011 $MAG Kpi $PARTICLE 
            else 
                sh PID_efficiency_split.sh 2011 $MAG Kpi $PARTICLE 
            fi
    done
done
