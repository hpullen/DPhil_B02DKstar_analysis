#!/usr/bin/env bash
function print_value {
    FILE=$1
    PAR_NAME=$2
    DP=$3
    VALUE=$(grep "^$PAR_NAME " $FILE | awk '{print $2}')
    VALUE_DP=$(printf %0.${DP}f\\n $VALUE)
    ERROR=$(grep "^$PAR_NAME " $FILE | awk '{print $3}')
    ERROR_DP=$(printf %0.${DP}f\\n $ERROR)
    echo "$VALUE_DP $\\pm$ $ERROR_DP"
}


# Make non resonant table
OUTFILE=/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Fit_setup/DKpipi_res.tex
FILE1=/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/DKpipi_1_Kstar_1680.param
FILE3=/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/DKpipi_3_K1_1400.param
FILE5=/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/DKpipi_5_Kstar_1410.param

# Start table
echo '\begin{table}[h]' > $OUTFILE
echo '    \centering' >> $OUTFILE
echo '    \begin{tabular}{lccc}' >> $OUTFILE
echo '        \toprule' >> $OUTFILE
echo '        Parameter & $K^*(1680)^+$ & K_1(1400)^+ & K^*(1410)^+ \\' >> $OUTFILE
echo '        \midrule' >> $OUTFILE

# Fill parameters
echo "      a & 5000 & 5000 & 5000 \\\\" >> $OUTFILE
echo "      a & 5139.69 & 5239.69 & 5139.69 \\\\" >> $OUTFILE
echo "      \$\\xi\$ & $(print_value $FILE1 csi 1) & $(print_value $FILE3 csi 1) & $(print_value $FILE5 csi 1) \\\\" >> $OUTFILE
echo "      \$\\sigma^{HILL}\$ & $(print_value $FILE1 sigma 1) & $(print_value $FILE3 sigma 1) & $(print_value $FILE5 sigma 1) \\\\" >> $OUTFILE
echo "      \$R\$ & $(print_value $FILE1 ratio 1) & $(print_value $FILE3 ratio 1) & $(print_value $FILE5 ratio 1) \\\\" >> $OUTFILE
echo "      \$f\$ & $(print_value $FILE1 frac 2) & $(print_value $FILE3 frac 2) & $(print_value $FILE5 frac 2) \\\\" >> $OUTFILE
echo "      \$\\sigma_1^{gauss}\$ & $(print_value $FILE1 sigma_gauss 1) & $(print_value $FILE3 sigma_gauss 1) & $(print_value $FILE5 sigma_gauss 1) \\\\" >> $OUTFILE
echo "      \$\\sigma_2^{gauss}\$ & $(print_value $FILE1 sigma_gauss2 1) & $(print_value $FILE3 sigma_gauss2 1) & $(print_value $FILE5 sigma_gauss2 1) \\\\" >> $OUTFILE
echo "      \$\\mu^{gauss}\$ & $(print_value $FILE1 mean_gauss 0) & $(print_value $FILE3 mean_gauss 0) & $(print_value $FILE5 mean_gauss 0) \\\\" >> $OUTFILE
echo "      \$f^{gauss}\$ & $(print_value $FILE1 f_gauss 2) & $(print_value $FILE3 f_gauss 2) & $(print_value $FILE5 f_gauss 2) \\\\" >> $OUTFILE

# End table
echo '  \bottomrule' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo '\end{table}' >> $OUTFILE
