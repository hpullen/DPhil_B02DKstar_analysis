# Apply all BDTs
for MAG in up down; do
    for YEAR in 2011 2012; do

    # Four-body
    BASE_DIR=/data/lhcb/users/pullen/B02DKstar/data/
    DIR=${BASE_DIR}/fourBody/${YEAR}_${MAG}/
    MODE=pipipipi
    BDT_MODE=pipipipi
    ./ApplyMVA ${BDT_MODE}_run2 $DIR/${MODE}.root ${DIR}/${MODE}_withBDTG.root 
    echo Applied MVA to $MODE $YEAR $MAG

    done
done
