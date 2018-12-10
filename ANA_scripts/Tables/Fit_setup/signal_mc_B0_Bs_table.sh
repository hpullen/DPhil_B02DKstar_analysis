#!/usr/bin/env bash 
source ../s_no.sh
function print_value {
    PAR_NAME=$1
    LATEX=$2
    VALUE1=$(grep $PAR_NAME $B0_FILE | awk '{print $2}')
    ERROR1=$(grep $PAR_NAME $B0_FILE | awk '{print $3}')
    VALUE2=$(grep $PAR_NAME $Bs_FILE | awk '{print $2}')
    ERROR2=$(grep $PAR_NAME $Bs_FILE | awk '{print $3}')
    echo "      \$$LATEX\$ & $(n_no $VALUE1 $ERROR1) & $(n_no $VALUE2 $ERROR2) \\\\"
}


# Results file
B0_FILE=/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/signal_Kpi_cruijff.param
Bs_FILE=/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/signal_Bs_cruijff.param
OUTFILE=/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Fit_setup/signal_MC_params.tex

# Set up table
echo '\begin{table}[h]' > $OUTFILE
echo '  \centering' >> $OUTFILE
echo '  \begin{tabular}{ccc}' >> $OUTFILE
echo '      \toprule' >> $OUTFILE
echo '      Parameter & $B^0$ value & $B^0_s$ value \\' >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# Print each parameter
print_value mean '\mu'>> $OUTFILE
print_value sigma_L '\sigma_L'>> $OUTFILE
print_value sigma_ratio 'R_\sigma'>> $OUTFILE
print_value alpha_L '\alpha_L'>> $OUTFILE
print_value alpha_R '\alpha_R'>> $OUTFILE

# Finish table
echo '  \bottomrule' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo '  \caption{Parameters obtained from fitting a Cruijff function' \
    'to samples of $B^0 \to DK^{*0}$ and $B^0_s \to DK^{*0}$ Monte Carlo'\
    ' with $D \to K\pi$.}'>> $OUTFILE
echo "\\label{tab:signal_MC_params}" >> $OUTFILE
echo '\end{table}' >> $OUTFILE
