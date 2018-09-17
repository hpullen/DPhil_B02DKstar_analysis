####################################################################################
# Makes a tex file containing a table of selection efficiencies for each mode
####################################################################################
source ../s_no.sh
# Location of analysis directory and table to create
RUN=$1
ANALYSIS_DIR="/home/pullen/analysis/B02DKstar/"
TAB_FILE="${ANALYSIS_DIR}/ANA_resources/Tables/Monte_carlo/selection_efficiency_${RUN}.tex"
ACC_FILE="${ANALYSIS_DIR}/Efficiencies/Values/acceptance_efficiency_${RUN}.param"
EFF_FILE="${ANALYSIS_DIR}/Efficiencies/Values/selection_efficiency_${RUN}.param"
TOT_FILE="${ANALYSIS_DIR}/Efficiencies/Values/total_efficiency_${RUN}.param"

# Remove table if it already exists
if [ -f $TAB_FILE ]; then
    rm $TAB_FILE
fi

print_line() {

    MODE=$1
    LATEX_FORM=$2
    echo "        \$${LATEX_FORM}\$ &"\
        "$(n_no $(grep "^$MODE\ " $ACC_FILE | awk '{print $2 * 100, $3 * 100}'))"\
        "& $(n_no $(grep "^$MODE\ " $EFF_FILE | awk '{print $2 * 100, $3 * 100}'))"\
        "& $(n_no $(grep "^$MODE\ " $TOT_FILE | awk '{print $2 * 100, $3 * 100}'))"\
         "\\\\" >> $TAB_FILE
}

# Print Latex table to file
echo '\begin{table}[H]'                                          >> $TAB_FILE
echo '    \centering'                                            >> $TAB_FILE
echo '    \begin{tabular}{cccc}'                                  >> $TAB_FILE
echo '        \toprule'                                          >> $TAB_FILE
echo '        $D$ mode & $\epsilon_\mathrm{acc}$(\%) & '\
             '$\epsilon_\mathrm{sel}$(\%) & '\
             '$\epsilon_\mathrm{tot}$(\%) \\'                    >> $TAB_FILE
echo '        \midrule'                                          >> $TAB_FILE
print_line Kpi 'K\pi'
print_line KK 'KK'
print_line pipi '\pi\pi'
print_line Kpi 'K\pi'
print_line Kpipipi 'K\pi\pi\pi'
if [[ $RUN == "run2" ]]; then
    print_line pipipipi '\pi\pi\pi\pi' 
fi
echo '        \bottomrule'                                       >> $TAB_FILE
echo '    \end{tabular}'                                         >> $TAB_FILE
if [[ $RUN == "run1" ]]; then
    RUN_TEXT="Run 1"
else 
    RUN_TEXT="Run 2"
fi
echo "    \\caption{Geometric acceptance and selection efficiencies from "\
         "${RUN_TEXT} Monte Carlo. The total efficiency is the "\
         "product of these.}"                                    >> $TAB_FILE
echo "\\label{tab:selection_efficiency_$RUN}"                    >> $TAB_FILE
echo '\end{table}'                                               >> $TAB_FILE
