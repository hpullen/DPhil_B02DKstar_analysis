# Print value of each observable with statistical and systematic 
source "../s_no.sh"

print_one() {

    # Setup
    PAR=$1
    FIT_RESULTS="../../../Systematics/final_result_splitObs.param"

    # Run 1 and Run 2 pars
    if echo $PAR | grep "R_signal_piK" >/dev/null; then
        PAR1=$(echo $PAR | sed 's/plus/run1_plus/; s/minus/run1_minus/')
        PAR2=$(echo $PAR | sed 's/plus/run2_plus/; s/minus/run2_minus/')
    else 
        PAR1=${PAR}_run1
        PAR2=${PAR}_run2
    fi

    # Get run 1 value 
    if echo $PAR | grep "pipipipi" > /dev/null; then
        VAL1="--"
    else 
        VAL1=$(awk "/^$PAR1 /{printf \"%.3f\", \$2}" $FIT_RESULTS)
        STAT1=$(awk "/^$PAR1 /{printf \"%.3f\", \$3}" $FIT_RESULTS)
        SYS1=$(awk "/^$PAR1 /{printf \"%.3f\", \$4}" $FIT_RESULTS)
        VAL1="${VAL1} \$\\pm\$ ${STAT1} \$\\pm\$ ${SYS1}"
    fi
    VAL2=$(awk "/^$PAR2 /{printf \"%.3f\", \$2}" $FIT_RESULTS)
    STAT2=$(awk "/^$PAR2 /{printf \"%.3f\", \$3}" $FIT_RESULTS)
    SYS2=$(awk "/^$PAR2 /{printf \"%.3f\", \$4}" $FIT_RESULTS)
    VAL2="${VAL2} \$\\pm\$ ${STAT2} \$\\pm\$ ${SYS2}"

    # Print
    echo "$VAL1 & $VAL2"

}


print_table() {

    # Start list
    OUTFILE="../../../ANA_resources/Tables/Systematics/results_split.tex"
    echo '\begin{table}' > $OUTFILE
    echo '\centering' >> $OUTFILE
    echo "\\caption{Measured observables split by LHC running period.}" >> $OUTFILE
    echo '\begin{tabular}{ccc}' >> $OUTFILE
    echo 'Observable & Run 1 & Run 2\\' >> $OUTFILE
    echo '\midrule' >> $OUTFILE

    # Fill
    NAMES_FILE="../../names_paper.param"
    for PAR in $*; do
        LATEX=$(grep "^$PAR " $NAMES_FILE | sed "s/^${PAR} //")
        echo "$LATEX & $(print_one $PAR) \\\\" >> $OUTFILE
    done

    # End
    echo '\end{tabular}' >> $OUTFILE
    echo "\\label{tab:results_split}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE

}

cd ../../../Systematics/ && ./FinalResult --splitObs && cd -
print_table A_signal_KK A_signal_pipi R_signal_KK R_signal_pipi A_signal_pipipipi R_signal_pipipipi R_signal_piK_plus R_signal_piK_minus R_signal_piKpipi_plus R_signal_piKpipi_minus A_signal_Kpi A_signal_Kpipipi A_Bs_piK A_Bs_piKpipi A_Bs_KK A_Bs_pipi R_Bs_KK R_Bs_pipi A_Bs_pipipipi R_Bs_pipipipi
