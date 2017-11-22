#!/usr/bin/bash
####################################################################################
# Makes a tex file containing a table of PID efficiencies for each year and polarity
####################################################################################
# Location of analysis directory and table to create
ANALYSIS_DIR="/home/pullen/analysis/B02DKstar/"
MODE=$1
TAB_FILE="${ANALYSIS_DIR}/ANA_tables/PID_efficiency_table_${MODE}.tex"
EFF_FILE="${ANALYSIS_DIR}/Efficiencies/Values/PID_efficiency.txt"
AV_FILE="${ANALYSIS_DIR}/Efficiencies/Values/PID_efficiency_average.txt"

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
echo '        & MagDown & MagUp \\'             >> $TAB_FILE
echo '        \midrule'                         >> $TAB_FILE
echo "        2015 & $(grep $MODE' 2015 down' $EFF_FILE | awk '{printf "%.2f", 100 * $4}')"\
     "& $(grep $MODE' 2015 up' $EFF_FILE | awk '{printf "%.2f", 100 * $4}')"\
     "\\\\"                                     >> $TAB_FILE
echo "        2016 & $(grep $MODE' 2016 down' $EFF_FILE | awk '{printf "%.2f", 100 * $4}')"\
     "& $(grep $MODE' 2016 up' $EFF_FILE | awk '{printf "%.2f", 100 * $4}')"\
     "\\\\"                                     >> $TAB_FILE
echo '        \midrule'                         >> $TAB_FILE
echo "        Run 2 combined &"\
     "\\multicolumn{2}{$(grep $MODE' ' $AV_FILE | awk '{printf "%.2f", 100 * $2}')}"\
     "\\\\"                                     >> $TAB_FILE
echo '        \bottomrule'                      >> $TAB_FILE
if [ $MODE == "Kpi" ]; then
    DECAY_STR='$D \to K\pi$'
elif [ $MODE == "KK" ]; then 
    DECAY_STR='$D \to KK$'
elif [ $MODE == "pipi" ]; then
    DECAY_TRE='$D \to \pi\pi$'
fi
echo '    \end{tabular}'                        >> $TAB_FILE
echo "    \\caption{PID efficiencies for ${DECAY_STR} for 2015 and 2016 for both"\
     "magnet polarities, given as percentages.}" >> $TAB_FILE
echo '\label{tab:PID_efficiency}'               >> $TAB_FILE
echo '\end{table}'                              >> $TAB_FILE
echo '\end{document}'                           >> $TAB_FILE
