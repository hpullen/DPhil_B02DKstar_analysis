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


TYPE=$1
DESC=$2

# Results file
FILE=$HOME/cernbox/temp_pplx/mc_fits/Fit_monte_carlo/Results/signal_$TYPE.param
OUTFILE=$HOME/cernbox/temp_pplx/ANA_tables/Tables/signal_${TYPE}_MC_params.tex

# Set up table
echo '\begin{table}[h]' > $OUTFILE
echo '  \centering' >> $OUTFILE
echo '  \begin{tabular}{lc}' >> $OUTFILE
echo '      \toprule' >> $OUTFILE
echo '      Parameter & Value \\' >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# Print each parameter
print_value $FILE mean '\mu' 2 >> $OUTFILE
print_value $FILE sigma_L '\sigma_L' 1 >> $OUTFILE
print_value $FILE sigma_ratio 'R' 3 >> $OUTFILE
print_value $FILE frac 'f' 3 >> $OUTFILE
print_value $FILE alpha_L '\alpha_L' 2 >> $OUTFILE
print_value $FILE alpha_R '\alpha_R' 2 >> $OUTFILE
print_value $FILE n_L 'n_L' 2 >> $OUTFILE
print_value $FILE n_R 'n_R' 2 >> $OUTFILE

# Finish table
echo '  \bottomrule' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo "  \\caption{Parameters obtained from fitting the sum of two Crystal"\
    "Ball PDFs to a sample of ${DESC} Monte Carlo.}" >> $OUTFILE
echo "\\label{tab:signal_${TYPE}_MC_params}" >> $OUTFILE
echo '\end{table}' >> $OUTFILE
