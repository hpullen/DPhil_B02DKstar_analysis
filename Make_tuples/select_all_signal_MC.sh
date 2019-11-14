#!/usr/bin/env bash
# Signal
for MODE in Kpi KK pipi Kpipipi pipipipi; do
    YEARS="2017 2018"
    # if [[ $MODE != "pipipipi" ]]; then
        # YEARS="${YEARS} 2012"
        # if [[ $MODE != "Kpipipi" ]]; then
            # YEARS="${YEARS} 2015"
            # if [[ $MODE == "Kpi" ]]; then
                # YEARS="${YEARS} 2011"
            # fi
        # fi
    # fi
    BOD="twoBody"
    if [[ $MODE == "Kpipipi" || $MODE == "pipipipi" ]]; then
        BOD="fourBody"
    fi
    for YEAR in $YEARS; do
        for MAG in up down; do
            ./MakeSelectedMCtuple $BOD/$MODE $YEAR $MAG
        done
    done
done

# Bs
# for YEAR in 2011 2012 2015 2016; do
    # for MAG in up down; do
        # ./MakeSelectedMCtuple backgrounds/Bs $YEAR $MAG
    # done
# done
