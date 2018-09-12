#!/usr/bin/env bash 
source ../s_no.sh
function print_value {
    FILE=$1
    PAR_NAME=$2
    LATEX=$3
    VALUE=$(grep $PAR_NAME $FILE | awk '{print $2}')
    ERROR=$(grep $PAR_NAME $FILE | awk '{print $3}')
    echo "      \$$LATEX\$ & $(n_no $VALUE $ERROR) \\\\"
}


TYPE=$1
DESC=$2

# Results file
FILE=/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/signal_${TYPE}.param
OUTFILE=/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Fit_setup/signal_${TYPE}_MC_params.tex

# Set up table
echo '\begin{table}[h]' > $OUTFILE
echo '  \centering' >> $OUTFILE
echo '  \begin{tabular}{lc}' >> $OUTFILE
echo '      \toprule' >> $OUTFILE
echo '      Parameter & Value \\' >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# Print each parameter
print_value $FILE mean '\mu'>> $OUTFILE
print_value $FILE sigma_L '\sigma'>> $OUTFILE
print_value $FILE frac 'f'>> $OUTFILE
print_value $FILE alpha_L '\alpha_L'>> $OUTFILE
print_value $FILE alpha_R '\alpha_R'>> $OUTFILE
print_value $FILE n_L 'n_L'>> $OUTFILE
print_value $FILE n_R 'n_R'>> $OUTFILE

# Finish table
echo '  \bottomrule' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo "  \\caption{Parameters obtained from fitting the sum of two Crystal"\
    "Ball PDFs to a sample of ${DESC} Monte Carlo.}" >> $OUTFILE
echo "\\label{tab:signal_${TYPE}_MC_params}" >> $OUTFILE
echo '\end{table}' >> $OUTFILE
