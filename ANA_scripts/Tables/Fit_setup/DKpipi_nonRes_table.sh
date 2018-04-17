#!/usr/bin/env bash
function print_value {
    FILE=$1
    PAR_NAME=$2
    LATEX=$3
    DP=$4
    VALUE=$(grep $PAR_NAME $FILE | awk '{print $2}')
    VALUE_DP=$(printf %0.${DP}f\\n $VALUE)
    ERROR=$(grep $PAR_NAME $FILE | awk '{print $3}')
    ERROR_DP=$(printf %0.${DP}f\\n $ERROR)
    echo "      \$$LATEX\$ & $VALUE_DP $\\pm$ $ERROR_DP \\\\"
}


# Make non resonant table
FILE1=/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Fit_setup/DKpipi_nonRes.tex
INFILE=/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/DKpipi_2_Kstar_892.param

# Start table
echo '\begin{table}[h]' > $FILE1
echo '    \centering' >> $FILE1
echo '    \begin{tabular}{lc}' >> $FILE1
echo '        \toprule' >> $FILE1
echo '        Parameter & Value \\' >> $FILE1
echo '        \midrule' >> $FILE1

# Fill parameters
echo "        a & $(grep '^a ' $INFILE | awk '{print $2}') \\\\" >> $FILE1
echo "        b & $(grep '^b ' $INFILE | awk '{print $2}') \\\\" >> $FILE1
print_value $INFILE csi '\xi' 1 >> $FILE1
print_value $INFILE sigma '\sigma' 1 >> $FILE1
print_value $INFILE ratio 'R' 1 >> $FILE1
print_value $INFILE frac 'f' 2 >> $FILE1
print_value $INFILE shift '\mathrm{Shift}' 1 >> $FILE1

# End table
echo '  \bottomrule' >> $FILE1
echo '  \end{tabular}' >> $FILE1
echo '\end{table}' >> $FILE1
