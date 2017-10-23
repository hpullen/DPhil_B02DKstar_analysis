for mag in "up" "down"; do
    ./../MakeTrainingTuple MC 2012 $mag fourBody Kpipipi Tuple_K3pi
    ./../MakeTrainingTuple MC 2016 $mag fourBody Kpipipi Tuple_K3pi
    ./../MakeTrainingTuple MC 2016 $mag fourBody pipipipi Tuple_4pi
done

