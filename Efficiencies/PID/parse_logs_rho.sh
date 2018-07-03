#!/usr/bin/env bash 
# Clear existing parameter files
if [[ -f Results/rho.param ]]; then
    rm Results/rho.param
fi

# Fill with values
for YEAR in 2012 2016; do
    for MAG in up down; do
        LOGFILE=logs/${YEAR}_${MAG}/rho.txt
        if [[ -f $LOGFILE ]]; then
            EFF=$(grep 'event efficiency' $LOGFILE | awk '{print $6}' | sed 's/(//')
            ERR=$(grep 'event efficiency' $LOGFILE | awk '{print $8}' | sed 's/)%//')
            echo "${YEAR}_${MAG} $EFF $ERR" >> Results/rho.param
        fi
    done
done
