for mag in "up" "down"; do

    # Two body signal
    for mode in "KK" "Kpi" "pipi"; do
        ./../../../MakeSelectedTuple MC 2015 $mag twoBody $mode 
        ./../../../MakeSelectedTuple MC 2016 $mag twoBody $mode 
    done

    # Four body signal
    for mode in "Kpipipi" "pipipipi"; do
        ./../../../MakeSelectedTuple MC 2015 $mag fourBody $mode 
        ./../../../MakeSelectedTuple MC 2016 $mag fourBody $mode 
    done

    # Bs
    ./../../../MakeSelectedTuple /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs 2015 $mag
    ./../../../MakeSelectedTuple /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs 2016 $mag

done
