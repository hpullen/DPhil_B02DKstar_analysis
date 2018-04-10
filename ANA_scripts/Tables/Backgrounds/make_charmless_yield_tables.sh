TIGHT_CUT=3

#!/usr/bin/env bash 
function print_line {
    DAUGHTERS=$1
    PARENT=$2
    RUN=$3
    LATEX_NAME=$4
    FILE="/home/pullen/analysis/B02DKstar/Backgrounds/Charmless/Results/${DAUGHTERS}_run_${RUN}.txt"
    CUT_0=$(printf "%.1f" $(grep "0 ${PARENT}" $FILE | awk '{print $3}'))
    CUT_0_ERR=$(printf "%.1f" $(grep "0 ${PARENT}" $FILE | awk '{print $4}'))
    CUT_TIGHT=$(printf "%.1f" $(grep "${TIGHT_CUT} ${PARENT}" $FILE | awk '{print $3}'))
    CUT_TIGHT_ERR=$(printf "%.1f" $(grep "${TIGHT_CUT} ${PARENT}" $FILE | awk '{print $4}'))
    echo "\$${LATEX_NAME}\$ & \$${CUT_0} \\pm ${CUT_0_ERR}\$ & \$${CUT_TIGHT} \\pm ${CUT_TIGHT_ERR}\$ \\\\"
}

for RUN in 1 2; do
    for MODE in Bd Bs; do
        OUTFILE="/home/pullen/analysis/B02DKstar/ANA_tables/Tables/Backgrounds/Charmless/yields_run_${RUN}_${MODE}.tex"
        echo '\begin{table}[h]' > $OUTFILE
        echo '  \centering' >> $OUTFILE
        echo '  \begin{tabular}{lll}' >> $OUTFILE
        echo '      \toprule' >> $OUTFILE
        echo "      Mode & $\\mathrm{FDS} > 0$ & $\\mathrm{FDS} > ${TIGHT_CUT}\\sigma$ \\\\" >> $OUTFILE
        echo '      \midrule' >> $OUTFILE
        if [[ "$MODE" == "Bd" ]]; then
            echo "      $(print_line Kpi $MODE $RUN 'K\pi')" >> $OUTFILE
        else 
            echo "      \$K\\pi\$ & \\-- & \\-- \\\\" >> $OUTFILE
        fi
        echo "      $(print_line piK $MODE $RUN '\pi K')" >> $OUTFILE
        echo "      $(print_line KK $MODE $RUN 'KK')" >> $OUTFILE
        echo "      $(print_line pipi $MODE $RUN '\pi\pi')" >> $OUTFILE
        if [[ "$MODE" == "Bd" ]]; then
            echo "      $(print_line Kpipipi $MODE $RUN 'K\pi\pi\pi')" >> $OUTFILE
        else 
            echo "      \$K\\pi\\pi\\pi\$ & \\-- & \\-- \\\\" >> $OUTFILE
        fi
        echo "      $(print_line piKpipi $MODE $RUN '\pi K\pi\pi')" >> $OUTFILE
        if [[ "$RUN" == "2" ]]; then
            echo "      $(print_line pipipipi $MODE $RUN '\pi\pi\pi\pi')" >> $OUTFILE
        fi
        echo '      \bottomrule' >> $OUTFILE
        echo '  \end{tabular}' >> $OUTFILE
        if [[ "$MODE" == "Bd" ]]; then
            MODE_LATEX='$B^{0}_{d}$'
        else 
            MODE_LATEX='$B^{0}_{s}$'
        fi
        echo "  \\caption{\\small Estimated charmless contribution to the ${MODE_LATEX}"\
             "yield in Run $RUN data for flight distance significance cuts of 0 "\
             "and \$${TIGHT_CUT}\\sigma\$.}" >> $OUTFILE
        echo "\\label{tab:charmless_yields_${MODE}_run_${RUN}}" >> $OUTFILE
        echo '\end{table}' >> $OUTFILE
    done
done
