####################################################################################
# Makes a tex file containing a table of selection efficiencies for each mode
####################################################################################
# Location of analysis directory and table to create
ANALYSIS_DIR="/home/pullen/analysis/B02DKstar/"
TAB_FILE="${ANALYSIS_DIR}/ANA_tables/selection_efficiency_table.tex"
EFF_FILE="${ANALYSIS_DIR}/Efficiencies/Values/selection_efficiency_average.txt"
N_FILE="${ANALYSIS_DIR}/Efficiencies/Values/n_selected_total.txt"

# Remove table if it already exists
if [ -f $TAB_FILE ]; then
    rm $TAB_FILE
fi

# Print Latex table to file
echo '\documentclass{article}'                  >> $TAB_FILE
echo '\begin{document}'                         >> $TAB_FILE
echo '\begin{table}[H]'                         >> $TAB_FILE
echo '    \centering'                           >> $TAB_FILE
echo '    \begin{tabular}{ccc}'                 >> $TAB_FILE
echo '        \toprule'                         >> $TAB_FILE
echo '        & \multicol{2}{c}{Run 2} & \\'             >> $TAB_FILE
echo '        Decay mode & $\epsilon_{sel}$(\%) & $_{sel} \\'             >> $TAB_FILE
echo '        \midrule'                         >> $TAB_FILE
echo "        $K\\pi$ & $(grep 'Kpi' $EFF_FILE | awk '{printf "%.4f + %.4f", 100 * $3}')"\
     #"& $(grep 'Kpi' $N_FILE | awk '{print $2}')"\
     "\\\\"                                     >> $TAB_FILE
echo '        \bottomrule'                      >> $TAB_FILE
echo '    \end{tabular}'                        >> $TAB_FILE
echo "    \\caption{Selection efficiencies for  for 2015 and 2016 for both"\
     "magnet polarities, given as percentages.}" >> $TAB_FILE
echo '\label{tab:PID_efficiency}'               >> $TAB_FILE
echo '\end{table}'                              >> $TAB_FILE
echo '\end{document}'                           >> $TAB_FILE



