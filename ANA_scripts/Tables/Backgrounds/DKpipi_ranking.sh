#!/usr/bin/env bash 
# Script to make LaTeX table of the fraction of each component of DKpipi
INFILE=/home/pullen/analysis/B02DKstar/Backgrounds/DKpipi/Scripts/yield_fractions.txt
OUTFILE=/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/DKpipi_ranking.tex

# Start table
echo "\\begin{table}[h]" > $OUTFILE
echo "  \\centering" >> $OUTFILE
echo "  \\begin{tabular}{clc}" >> $OUTFILE
echo "      \\toprule" >> $OUTFILE
echo "      Rank & Decay & Fraction \\\\" >> $OUTFILE
echo "      \\midrule" >> $OUTFILE

# Add each component
echo "      1 & \$B^+ \\to K_1(1400)^+ (\\to K^{*0}\\pi^+) \\bar{D}^0\$ & $(grep "3_K1_1400/Kstar" $INFILE | awk '{print $2}') \\\\ " >> $OUTFILE
echo "      2 & \$B^+ \\to K^*(1400)^+ (\\to K^{*0}\\pi^+) \\bar{D}^0\$ & $(grep "5_Kstar_1410/Kstar" $INFILE | awk '{print $2}') \\\\ " >> $OUTFILE
echo "      3 & \$B^+ \\to K_1(1270)^+ (\\to K^{*0}\\pi^+) \\bar{D}^0\$ & $(grep "7_K1_1270/Kstar" $INFILE | awk '{print $2}') \\\\ " >> $OUTFILE
echo "      4 & \$B^+ \\to K^*(1680)^+ (\\to K^{*0}\\pi^+) \\bar{D}^0\$ & $(grep "1_Kstar_1680/Kstar" $INFILE | awk '{print $2}') \\\\ " >> $OUTFILE
echo "      5 & \$B^+ \\to K^{*0} \\pi^+ \\bar{D}^0\$ & $(grep "2_Kstar_892" $INFILE | awk '{print $2}') \\\\ " >> $OUTFILE
echo "      6 & \$B^+ \\to K_1(1400)^+ (\\to K^+ \\pi^- \\pi^+) \\bar{D}^0\$ & $(grep "3_K1_1400/non_res" $INFILE | awk '{print $2}') \\\\ " >> $OUTFILE
echo "      7 & \$B^+ \\to K^+ \\pi^- \\pi^+ \\bar{D}^0\$ & $(grep "10_nonres" $INFILE | awk '{print $2}') \\\\ " >> $OUTFILE


# End table
echo "      \\bottomrule" >> $OUTFILE
echo "  \\end{tabular}" >> $OUTFILE
echo "\\end{table}" >> $OUTFILE
