#!/usr/bin/env bash 
source ../s_no.sh
function print_value {
    FILE=$1
    PAR_NAME=$2
    LATEX=$3
    DP=$4
    VALUE=$(n_no $(grep $PAR_NAME $FILE | awk '{print $2, $3}'))
    echo "      \$$LATEX\$ & $VALUE \\\\"
}


# Results file
FILE=/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/rho.param
OUTFILE=/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Fit_setup/rho_mc_params.tex

# Set up table
echo '\begin{table}[h]' > $OUTFILE
echo '  \centering' >> $OUTFILE
echo '  \begin{tabular}{lc}' >> $OUTFILE
echo '      \toprule' >> $OUTFILE
echo '      Parameter & Value \\' >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# Print each parameter
print_value $FILE mean '\mu' 1 >> $OUTFILE
print_value $FILE sigma_L '\sigma_L' 1 >> $OUTFILE
print_value $FILE sigma_ratio 'R' 2 >> $OUTFILE
print_value $FILE frac 'f' 2 >> $OUTFILE
print_value $FILE alpha_L '\alpha_L' 2 >> $OUTFILE
print_value $FILE alpha_R '\alpha_R' 3 >> $OUTFILE
echo '$n_L$ & 1 (fixed) \\' >> $OUTFILE
print_value $FILE n_R 'n_R' 1 >> $OUTFILE

# Finish table
echo '  \bottomrule' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo "  \\caption{Parameters obtained from fit to \$B^0 \\to D\\rho^0\$ Monte "\
     "Carlo, adjusted to account for PID cuts, with the sum of two"\
     "Crystal Ball PDFs.}" >> $OUTFILE
echo "\\label{tab:rho_mc_params}" >> $OUTFILE
echo '\end{table}' >> $OUTFILE
