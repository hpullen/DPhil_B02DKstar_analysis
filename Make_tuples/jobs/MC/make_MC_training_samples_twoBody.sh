for mag in "up" "down"; do
    for mode in "KK" "Kpi" "pipi"; do
        if [[ mode == "Kpi" ]]; then
            ./../MakeTrainingTuple MC 2012 $mag twoBody $mode TupleB0ToD0KPi_D0ToKPi
        else
            ./../MakeTrainingTuple MC 2012 $mag twoBody $mode 
        fi
        ./../MakeTrainingTuple MC 2015 $mag twoBody $mode 
        ./../MakeTrainingTuple MC 2016 $mag twoBody $mode 
    done
done

