function print_value {
    FILE=$1
    PAR_NAME=$2
    DP=$3
    VALUE=$(grep "^$PAR_NAME" $FILE | awk '{print $2}')
    VALUE_DP=$(printf %0.${DP}f\\n $VALUE)
    ERROR=$(grep "^$PAR_NAME" $FILE | awk '{print $3}')
    ERROR_DP=$(printf %0.${DP}f\\n $ERROR)
    echo "$VALUE_DP $\\pm$ $ERROR_DP"
}

function print_all_values {
    FILE=$1
    PAR_ROOT=$2
    DP=$3
    GAMMA_010=$(print_value $FILE "${PAR_ROOT}_gamma_010" $DP)
    GAMMA_101=$(print_value $FILE "${PAR_ROOT}_gamma_101" $DP)
    PI_010=$(print_value $FILE "${PAR_ROOT}_pi_010" $DP)
    PI_101=$(print_value $FILE "${PAR_ROOT}_pi_101" $DP)
    echo "$GAMMA_010 & $GAMMA_101 & $PI_010 & $PI_101 \\\\"
}

FILE=$HOME/cernbox/temp_pplx/mc_fits/Fit_monte_carlo/Results/lowMass.param
OUTFILE=$HOME/cernbox/temp_pplx/ANA_tables/Tables/lowMass_mc_params.tex

# Start table
echo '\begin{table}[h]' > $OUTFILE
echo '    \centering' >> $OUTFILE
echo '    \begin{tabular}{ccccc}' >> $OUTFILE
echo '        \toprule' >> $OUTFILE
echo '        \multirow{2}{*}{Parameter} & \multicolumn{2}{c}{$D^0 \gamma$} &'\
     '\multicolumn{2}{c}{$D^0 \pi^0$} \\' >> $OUTFILE
echo '                                   & 010 & 101 & 010 & 101 \\' >> $OUTFILE
echo '        \midrule' >> $OUTFILE

# Frac and ratio shared for all
echo "      \$f\$ & \\multicolumn{4}{c}{$(print_value $FILE frac 3)} \\\\" >> $OUTFILE
echo "      \$R\$ & \\multicolumn{4}{c}{$(print_value $FILE ratio 2)} \\\\" >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# a and b values shared for 010/101
echo "      \$a\$ & \\multicolumn{2}{c}{$(grep '^a_gamma' $FILE | awk '{print $2}')}"\
    "& \\multicolumn{2}{c}{$(print_value $FILE a_pi 1)} \\\\" >> $OUTFILE
echo "      \$b\$ & \\multicolumn{2}{c}{$(print_value $FILE b_gamma 1)} &"\
    "\\multicolumn{2}{c}{$(print_value $FILE b_pi 1)} \\\\" >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# Other parameters
echo "      \$B^0\\ \\sigma\$ & $(print_all_values $FILE sigma 1)" >> $OUTFILE
echo "      \$B^0_s\\ \\sigma\$ & $(print_all_values $FILE Bs_sigma 1)" >> $OUTFILE
echo "      \$B^0\\ \\xi\$ & $(print_all_values $FILE csi 2)" >> $OUTFILE
echo "      \$B^0_s\\ \\xi\$ & $(print_all_values $FILE Bs_csi 2)" >> $OUTFILE

# End table
echo '        \bottomrule' >> $OUTFILE
echo '    \end{tabular}' >> $OUTFILE
echo '    \caption{Fit parameters obtained from fitting to samples of $B^0\to
D^*K^{*0}$ and $B^0_s \to D^*K^{*0}$ Monte Carlo, with $D^*$ final states $D^0\gamma$
and $D^*\pi^0$, and helicity states 010 and 101. The double Gaussian parameters $f$
and $R$ are shared between the eight categories. The kinematic endpoints $a$ and $b$
are common to $\gamma$ and $\pi^0$ final states. The endpoint $a_{\gamma}$ is fixed
to its theoretical value. The other parameters, $\sigma$ and $\xi$, are separate for each category.}' >> $OUTFILE
echo '\label{tab:lowMass_mc_params}' >> $OUTFILE
echo '\end{table}' >> $OUTFILE

