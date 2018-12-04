#!/usr/bin/env bash
B0_FILE="../Results/B0.txt"
echo "2011 $(./GetProductionAsymmetry 2011 B0 | awk '{print $3, $5}')" > $B0_FILE
echo "2012 $(./GetProductionAsymmetry 2012 B0 | awk '{print $3, $5}')" >> $B0_FILE

Bs_FILE="../Results/Bs.txt"
echo "2011 $(./GetProductionAsymmetry 2011 Bs | awk '{print $3, $5}')" > $Bs_FILE
echo "2012 $(./GetProductionAsymmetry 2012 Bs | awk '{print $3, $5}')" >> $Bs_FILE

# Average over years and polarities
./AverageAsymmetry ../Results/production_asymmetry_run1.param B0 $B0_FILE Bs $Bs_FILE --run1
./AverageAsymmetry ../Results/production_asymmetry_run2.param B0 $B0_FILE Bs $Bs_FILE --run2
