#!/usr/bin/env bash 
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR="/data/lhcb/users/pullen/B02DKstar/"
./TrainMVA pipipipi_run2 -s $TUPLEDIR/MC/fourBody/pipipipi/2016_down/pipipipi_trainingSample.root $TUPLEDIR/MC/fourBody/pipipipi/2016_up/pipipipi_trainingSample.root -b $TUPLEDIR/data/fourBody/2015_down/pipipipi_trainingSample.root $TUPLEDIR/data/fourBody/2015_up/pipipipi_trainingSample.root $TUPLEDIR/data/fourBody/2016_down/pipipipi_trainingSample.root $TUPLEDIR/data/fourBody/2016_up/pipipipi_trainingSample.root | tee logs/pipipipi_run2.log
cd -
