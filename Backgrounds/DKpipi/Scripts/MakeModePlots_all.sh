#!/usr/bin/env bash 
for i in 10_nonres 1_Kstar_1680/Kstar 1_Kstar_1680/rho 2_Kstar_892 3_K1_1400/Kstar 3_K1_1400/rho 3_K1_1400/non_res 4_Kstar_1430 5_Kstar_1410/Kstar 5_Kstar_1410/rho 6_rho 7_K1_1270/Kstar 7_K1_1270/rho
do
    root -b -q "MakeModePlots.C(\"$i\")"
done
