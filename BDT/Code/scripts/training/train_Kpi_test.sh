#!/usr/bin/env bash 
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR="/data/lhcb/users/pullen/B02DKstar/"
./TrainMVA Kpi_run2_test -s $TUPLEDIR/MC/twoBody/Kpi/2016_down/Kpi_trainingSample.root $TUPLEDIR/MC/twoBody/Kpi/2016_up/Kpi_trainingSample.root $TUPLEDIR/MC/twoBody/Kpi/2015_down/Kpi_trainingSample.root $TUPLEDIR/MC/twoBody/Kpi/2015_up/Kpi_trainingSample.root -b $TUPLEDIR/data/twoBody/2015_down/Kpi_trainingSample.root $TUPLEDIR/data/twoBody/2015_up/Kpi_trainingSample.root $TUPLEDIR/data/twoBody/2016_down/Kpi_trainingSample.root $TUPLEDIR/data/twoBody/2016_up/Kpi_trainingSample.root -v Kpi_run2| tee logs/Kpi_run2_withBDT.log
cd -
