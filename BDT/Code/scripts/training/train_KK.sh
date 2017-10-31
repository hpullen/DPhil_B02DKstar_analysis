#!/usr/bin/env bash 
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR="/data/lhcb/users/pullen/B02DKstar/"
./TrainMVA KK_run2 -s $TUPLEDIR/MC/twoBody/KK/2016_down/KK_trainingSample.root $TUPLEDIR/MC/twoBody/KK/2016_up/KK_trainingSample.root $TUPLEDIR/MC/twoBody/KK/2015_down/KK_trainingSample.root $TUPLEDIR/MC/twoBody/KK/2015_up/KK_trainingSample.root -b $TUPLEDIR/data/twoBody/2015_down/KK_trainingSample.root $TUPLEDIR/data/twoBody/2015_up/KK_trainingSample.root $TUPLEDIR/data/twoBody/2016_down/KK_trainingSample.root $TUPLEDIR/data/twoBody/2016_up/KK_trainingSample.root | tee logs/KK_run2.log
cd -
