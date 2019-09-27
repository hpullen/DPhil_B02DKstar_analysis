# Apply all BDTs
for MAG in up down; do

    # Two-body
    BASE_DIR=/data/lhcb/users/pullen/B02DKstar/data/
    DIR=${BASE_DIR}/twoBody/2018_${MAG}/
    for MODE in Kpi piK KK pipi; do
        BDT_MODE=$MODE
        if [[ $MODE == "piK" ]]; then BDT_MODE="Kpi"; fi
        ./ApplyMVA ${BDT_MODE}_run2 $DIR/${MODE}.root ${DIR}/${MODE}_withBDTG.root 
        echo Apply MVA to $MODE $MAG
    done

    # Four-body
    DIR=${BASE_DIR}/fourBody/2018_${MAG}/
    for MODE in Kpipipi piKpipi pipipipi; do
        BDT_MODE=$MODE
        if [[ $MODE == "piKpipi" ]]; then BDT_MODE="Kpipipi"; fi
        ./ApplyMVA ${BDT_MODE}_run2 $DIR/${MODE}.root ${DIR}/${MODE}_withBDTG.root 
        echo Apply MVA to $MODE $MAG
    done
done
