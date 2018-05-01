for MAG in MagUp MagDown; do
    for MODE in Kpi KK pipi; do
        for YEAR in 2012 2015 2016; do
            sh PID_efficiency.sh $YEAR $MAG $MODE | tee logs/${YEAR}_$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')/${MODE}.txt
        done
    done
    sh PID_efficiency.sh 2011 $MAG Kpi | tee logs/2011_$(echo $MAG | sed 's/Mag//' | awk '{print tolower($0)}')/Kpi.txt
done
