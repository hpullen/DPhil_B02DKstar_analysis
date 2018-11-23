#!/usr/bin/env bash 
# Loop through particles, years etc
for MODE in Kpi Kpipipi; do
    for PARTICLE in "_B0" "_B0bar"; do

        # Clear existing parameter files
        if [[ -f Results/${MODE}${PARTICLE}_doubleSwap.param ]]; then
            rm Results/${MODE}${PARTICLE}_doubleSwap.param
        fi

        # Fill with values
        for YEAR in 2011 2012 2015 2016; do
            for MAG in up down; do
                LOGFILE=logs/${YEAR}_${MAG}/${MODE}${PARTICLE}_doubleSwap.txt
                if [[ -f $LOGFILE ]]; then
                    EFF=$(grep 'event efficiency' $LOGFILE | awk '{print $6}' | sed 's/(//')
                    ERR=$(grep 'event efficiency' $LOGFILE | awk '{print $8}' | sed 's/)%//')
                    echo "${YEAR}_${MAG} $EFF $ERR" >> Results/${MODE}${PARTICLE}_doubleSwap.param
                fi
            done
        done
    done
done

