# Make table with DKpipi parameters
source ../s_no.sh

# Print one line
print_line() {
    MULTIROW=$1
    PAR=$2
    LATEX=$3
    if [[ $MULTIROW ]]; then 
        MULTIROW="\\multirow{2}{*}{$MULTIROW}"
    fi
    INFILE="/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/DKpipi.param"
    VALUE=$(n_no $(awk "/^$PAR /{print \$2, \$3}" $INFILE))
    echo "$MULTIROW & \$$LATEX\$ & $VALUE \\\\"
}

print_line_fixed() {
    MULTIROW=$1
    PAR=$2
    LATEX=$3
    if [[ $MULTIROW ]]; then 
        if [[ $MULTIROW == "RooHILLdini" ]]; then
            MULTIROW="\\multirow{6}{*}{$MULTIROW}"
        else 
            MULTIROW="\\multirow{3}{*}{$MULTIROW}"
        fi 
    fi
    INFILE="/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/DKpipi.param"
    VALUE=$(awk "/^$PAR /{print \$2}" $INFILE)
    echo "$MULTIROW & \$$LATEX\$ & $VALUE \\\\"
}

# Make full table
TAB_FILE="/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Fit_setup/DKpipi_params.tex"
echo '\begin{table}' > $TAB_FILE
echo '  \centering' >> $TAB_FILE
echo '  \begin{tabular}{ccc}' >> $TAB_FILE
echo '      \toprule' >> $TAB_FILE
echo '      Shape & Parameter & Value \\' >> $TAB_FILE
echo '      \midrule' >> $TAB_FILE
print_line_fixed 'RooHILLdini' a 'a' >> $TAB_FILE
print_line_fixed '' b 'b' >> $TAB_FILE
print_line '' sigma '\sigma' >> $TAB_FILE
print_line '' ratio 'R_\sigma' >> $TAB_FILE
print_line '' frac 'f' >> $TAB_FILE
print_line_fixed '' csi '\xi' >> $TAB_FILE
echo '      \midrule' >> $TAB_FILE
print_line 'Gaussian 1' mean_gauss1 '\mu' >> $TAB_FILE
print_line '' sigma_gauss1 '\sigma' >> $TAB_FILE
print_line '' f_gauss1 'f' >> $TAB_FILE
echo '      \midrule' >> $TAB_FILE
print_line 'Gaussian 2' mean_gauss2 '\mu' >> $TAB_FILE
print_line '' sigma_gauss2 '\sigma' >> $TAB_FILE
print_line '' f_gauss2 'f' >> $TAB_FILE
echo '      \bottomrule' >> $TAB_FILE
echo '  \end{tabular}' >> $TAB_FILE
echo '  \caption{Parameters obtained from fit to simulated $B^+ \to DK^+\pi^-\pi^+$ generated with RapidSim. The sum of RooHORNSdini and two Gaussians is used. The upper endpoint, $b$, is fixed to its theoretical value. The slope, $\xi$, is fixed to 1.}' >> $TAB_FILE
echo '\label{tab:DKpipi_parameters}' >> $TAB_FILE
echo '\end{table}' >> $TAB_FILE


