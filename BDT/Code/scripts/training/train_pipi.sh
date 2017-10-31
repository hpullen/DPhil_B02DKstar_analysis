#!/usr/bin/env bash 
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR="/data/lhcb/users/pullen/B02DKstar/"
./TrainMVA pipi_run2 -s $TUPLEDIR/MC/twoBody/pipi/2016_down/pipi_trainingSample.root $TUPLEDIR/MC/twoBody/pipi/2016_up/pipi_trainingSample.root $TUPLEDIR/MC/twoBody/pipi/2015_down/pipi_trainingSample.root $TUPLEDIR/MC/twoBody/pipi/2015_up/pipi_trainingSample.root -b $TUPLEDIR/data/twoBody/2015_down/pipi_trainingSample.root $TUPLEDIR/data/twoBody/2015_up/pipi_trainingSample.root $TUPLEDIR/data/twoBody/2016_down/pipi_trainingSample.root $TUPLEDIR/data/twoBody/2016_up/pipi_trainingSample.root | tee logs/pipi_run2.log
cd -
