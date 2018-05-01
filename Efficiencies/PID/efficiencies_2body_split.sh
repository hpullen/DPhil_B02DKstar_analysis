for MAG in MagUp MagDown; do
    for PARTICLE in B0 B0bar; do
        for MODE in Kpi KK pipi; do
            for YEAR in 2012 2015 2016; do
                sh PID_efficiency_split.sh $YEAR $MAG $MODE $PARTICLE | tee logs/${YEAR}_$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')/${MODE}_${PARTICLE}.txt
            done
        done
        sh PID_efficiency.sh 2011 $MAG Kpi $PARTICLE | tee logs/2011_$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')/Kpi_${PARTICLE}.txt
    done
done
