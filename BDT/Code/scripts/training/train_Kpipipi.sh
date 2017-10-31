#!/usr/bin/env bash 
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR="/data/lhcb/users/pullen/B02DKstar/"
./TrainMVA Kpipipi_run2 -s $TUPLEDIR/MC/fourBody/Kpipipi/2016_down/Kpipipi_trainingSample.root $TUPLEDIR/MC/fourBody/Kpipipi/2016_up/Kpipipi_trainingSample.root -b $TUPLEDIR/data/fourBody/2015_down/Kpipipi_trainingSample.root $TUPLEDIR/data/fourBody/2015_up/Kpipipi_trainingSample.root $TUPLEDIR/data/fourBody/2016_down/Kpipipi_trainingSample.root $TUPLEDIR/data/fourBody/2016_up/Kpipipi_trainingSample.root | tee logs/Kpipipi_run2.log
cd -
