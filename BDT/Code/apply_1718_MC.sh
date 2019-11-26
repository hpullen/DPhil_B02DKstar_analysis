# Apply all BDTs
for YEAR in 2017 2018; do
    for MAG in up down; do

        # Two-body
        BASE_DIR=/data/lhcb/users/pullen/B02DKstar/MC
        for MODE in Kpi KK pipi; do
            BDT_MODE=$MODE
            DIR=${BASE_DIR}/twoBody/${MODE}/${YEAR}_${MAG}/
            ./ApplyMVA ${BDT_MODE}_run2 $DIR/${MODE}.root ${DIR}/${MODE}_withBDTG.root
            echo "Applied MVA to $MODE $YEAR $MAG"
        done

        # Four-body
        # for MODE in Kpipipi pipipipi; do
            # BDT_MODE=$MODE
            # DIR=${BASE_DIR}/fourBody/${MODE}/${YEAR}_${MAG}/
            # ./ApplyMVA ${BDT_MODE}_run2 $DIR/${MODE}.root ${DIR}/${MODE}_withBDTG.root
            # echo "Applied MVA to $MODE $YEAR $MAG"
        # done
    done
done
