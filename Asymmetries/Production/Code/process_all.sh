#!/usr/bin/env bash
B0_FILE="../Results/B0.txt"
echo "2011_down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2011_down/Kpi_selected.root B0 7 | awk '{print $3, $5}')" > $B0_FILE
echo "2011_up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2011_up/Kpi_selected.root B0 7 | awk '{print $3, $5}')" >> $B0_FILE
echo "2012_down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2012_down/Kpi_selected.root B0 8 | awk '{print $3, $5}')" >> $B0_FILE
echo "2012_up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2012_up/Kpi_selected.root B0 8 | awk '{print $3, $5}')" >> $B0_FILE

Bs_FILE="../Results/Bs.txt"
echo "2011_down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2011_down/Kpi_selected.root Bs 7 | awk '{print $3, $5}')" > $Bs_FILE
echo "2011_up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2011_up/Kpi_selected.root Bs 7 | awk '{print $3, $5}')" >> $Bs_FILE
echo "2012_down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2012_down/Kpi_selected.root Bs 8 | awk '{print $3, $5}')" >> $Bs_FILE
echo "2012_up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2012_up/Kpi_selected.root Bs 8 | awk '{print $3, $5}')" >> $Bs_FILE

# Average over years and polarities
./AverageAsymmetry ../Results/production_asymmetry.param B0 $B0_FILE Bs $Bs_FILE
./AverageAsymmetry ../Results/production_asymmetry_run1.param B0 $B0_FILE Bs $Bs_FILE --run1
./AverageAsymmetry ../Results/production_asymmetry_run2.param B0 $B0_FILE Bs $Bs_FILE --run2
