# Print value of each observable with statistical and systematic 
source "../s_no.sh"

print_one() {

    # Setup
    PAR=$1
    FIT_RESULTS="../../../Systematics/final_result.param"

    # Get value (or ??? if blind) and stat uncertainty
    if echo $PAR | grep "blind" > /dev/null; then
        VAL="??"
    else 
        VAL=$(awk "/^$PAR /{printf \"%.3f\", \$2}" $FIT_RESULTS)
    fi
    STAT=$(awk "/^$PAR /{printf \"%.3f\", \$3}" $FIT_RESULTS)
    SYS=$(awk "/^$PAR /{printf \"%.3f\", \$4}" $FIT_RESULTS)
    VAL="${VAL} \$\\pm\$ ${STAT} \$\\pm\$ ${SYS}"

    # Print
    echo $VAL

}


print_list() {

    # Start list
    NAME=$1
    shift
    OUTFILE="../../../ANA_resources/Tables/Systematics/paper_${NAME}_final_values.tex"
    echo '\begin{enumerate}' > $OUTFILE

    # Fill
    NAMES_FILE="../../names_paper.param"
    for PAR in $*; do
        LATEX=$(grep "^$PAR " $NAMES_FILE | sed "s/^${PAR} //")
        echo "\\item $LATEX = $(print_one $PAR)" >> $OUTFILE
        # print_one $PAR
    done

    # End
    echo '\end{enumerate}' >> $OUTFILE

}


cd ../../../Systematics/ && ./FinalResult && cd -
print_list principal A_signal_KK A_signal_pipi R_signal_KK R_signal_pipi A_signal_pipipipi R_signal_pipipipi R_signal_piK_plus R_signal_piK_minus R_signal_piKpipi_plus R_signal_piKpipi_minus
print_list other A_signal_Kpi A_signal_Kpipipi A_Bs_piK A_Bs_piKpipi A_Bs_KK A_Bs_pipi R_Bs_KK R_Bs_pipi A_Bs_pipipipi R_Bs_pipipipi
