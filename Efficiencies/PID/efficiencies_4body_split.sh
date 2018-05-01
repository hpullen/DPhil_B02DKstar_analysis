for MAG in MagUp MagDown; do
    for PARTICLE in B0 B0bar; do
        sh PID_efficiency_split.sh 2012 $MAG Kpipipi $PARTICLE | tee logs/2012_$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')/Kpipipi_${PARTICLE}.txt
        sh PID_efficiency_split.sh 2016 $MAG Kpipipi $PARTICLE | tee logs/2016_$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')/Kpipipi_${PARTICLE}.txt
        sh PID_efficiency_split.sh 2016 $MAG pipipipi $PARTICLE | tee logs/2016_$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')/pipipipi_${PARTICLE}.txt
    done
done
