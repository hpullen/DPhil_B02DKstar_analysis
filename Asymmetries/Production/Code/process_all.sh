#!/usr/bin/env bash 
B0_FILE="../Results/B0_separate_errors.txt"
echo "2011_down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2011_down/Kpi_selected.root B0 7 | awk '{print $3, $5, $8}')" > $B0_FILE
echo "2011_up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2011_up/Kpi_selected.root B0 7 | awk '{print $3, $5, $8}')" >> $B0_FILE
echo "2012_down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2012_down/Kpi_selected.root B0 8 | awk '{print $3, $5, $8}')" >> $B0_FILE
echo "2012_up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/2012_up/Kpi_selected.root B0 8 | awk '{print $3, $5, $8}')" >> $B0_FILE

Bs_FILE="../Results/Bs_separate_errors.txt"
echo "2011_down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2011_down/Kpi_selected.root Bs 7 | awk '{print $3, $5, $8}')" > $Bs_FILE
echo "2011_up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2011_up/Kpi_selected.root Bs 7 | awk '{print $3, $5, $8}')" >> $Bs_FILE
echo "2012_down $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2012_down/Kpi_selected.root Bs 8 | awk '{print $3, $5, $8}')" >> $Bs_FILE
echo "2012_up $(./GetProductionAsymmetry /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs/2012_up/Kpi_selected.root Bs 8 | awk '{print $3, $5, $8}')" >> $Bs_FILE

# Combine statistical and systematic uncertainties in quadrature
B0_FILE2="../Results/B0.txt"
Bs_FILE2="../Results/Bs.txt"
awk '{print $1, $2, sqrt($3 ^ 2 + $4 ^ 2)}' $B0_FILE > $B0_FILE2
awk '{print $1, $2, sqrt($3 ^ 2 + $4 ^ 2)}' $Bs_FILE > $Bs_FILE2

# Average over years and polarities
./AverageAsymmetry ../Results/production_asymmetry.param B0 $B0_FILE2 Bs $Bs_FILE2
