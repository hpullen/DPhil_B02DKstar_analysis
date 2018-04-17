#!/usr/bin/env bash 
B0_FILE="../Results/B0.txt"
echo "2011 down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2011_down/Kpi.root B0 7 | awk '{print $3, $4, $5}')" > $B0_FILE
echo "2011 up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2011_up/Kpi.root B0 7 | awk '{print $3, $4, $5}')" >> $B0_FILE
echo "2012 down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2012_down/Kpi.root B0 8 | awk '{print $3, $4, $5}')" >> $B0_FILE
echo "2012 up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2012_up/Kpi.root B0 8 | awk '{print $3, $4, $5}')" >> $B0_FILE

Bs_FILE="../Results/Bs.txt"
echo "2011 down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2011_down/Kpi.root Bs 7 | awk '{print $3, $4, $5}')" > $Bs_FILE
echo "2011 up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2011_up/Kpi.root Bs 7 | awk '{print $3, $4, $5}')" >> $Bs_FILE
echo "2012 down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2012_down/Kpi.root Bs 8 | awk '{print $3, $4, $5}')" >> $Bs_FILE
echo "2012 up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2012_up/Kpi.root Bs 8 | awk '{print $3, $4, $5}')" >> $Bs_FILE
