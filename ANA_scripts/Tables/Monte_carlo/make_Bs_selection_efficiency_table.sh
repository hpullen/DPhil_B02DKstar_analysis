####################################################################################
# Makes a tex file containing a table of selection efficiencies for each mode
####################################################################################
source ../s_no.sh
# Location of analysis directory and table to create
RUN=$1
ANALYSIS_DIR="/home/pullen/analysis/B02DKstar/"
TAB_FILE="${ANALYSIS_DIR}/ANA_resources/Tables/Monte_carlo/selection_efficiency_Bs.tex"

# Remove table if it already exists
if [ -f $TAB_FILE ]; then
    rm $TAB_FILE
fi

print_line() {

    RUN=$1
    ACC_FILE="${ANALYSIS_DIR}/Efficiencies/Values/acceptance_efficiency_Bs_run${RUN}.param"
    EFF_FILE="${ANALYSIS_DIR}/Efficiencies/Values/selection_efficiency_Bs_run${RUN}.param"
    TOT_FILE="${ANALYSIS_DIR}/Efficiencies/Values/total_efficiency_Bs_run${RUN}.param"
    echo "        Run $RUN &"\
        "$(grep "^Kpi\ " $ACC_FILE | awk '{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}')"\
        "& $(n_no $(grep "^Kpi\ " $EFF_FILE | awk '{print $2 * 100, $3 * 100}'))"\
        "$(grep "^Kpi\ " $TOT_FILE | awk '{printf "$%.3f \\pm %.3f$", $2 * 100, $3 * 100}')"\
         "\\\\" >> $TAB_FILE
}

# Print Latex table to file
echo '\begin{table}'                                          >> $TAB_FILE
echo '    \centering'                                            >> $TAB_FILE
echo '    \begin{tabular}{cccc}'                                  >> $TAB_FILE
echo '        \toprule'                                          >> $TAB_FILE
echo '        & $\epsilon_\mathrm{acc}$(\%) & '\
             '$\epsilon_\mathrm{sel}$(\%) & '\
             '$\epsilon_\mathrm{tot}$(\%) \\'                    >> $TAB_FILE
echo '        \midrule'                                          >> $TAB_FILE
print_line 1
print_line 2
echo '        \bottomrule'                                       >> $TAB_FILE
echo '    \end{tabular}'                                         >> $TAB_FILE
echo '    \caption{Geometric acceptance and selection efficiencies from '\
         '$B_s \to DK^{*0}$ Monte Carlo with the $K\pi$ final state. '\
         'The total efficiency is the product of these.}'       >> $TAB_FILE
echo "\\label{tab:selection_efficiency_Bs}"                    >> $TAB_FILE
echo '\end{table}'                                               >> $TAB_FILE
