# Print value of each observable with statistical and systematic 
source "../s_no.sh"

print_one() {

    # Setup
    PAR=$1
    FIT_RESULTS="../../../Systematics/final_result.param"
    if grep 'ADS' <<< $PAR >/dev/null; then
        FIT_RESULTS="../../../Systematics/ADS_results.param"
    fi

    # Get value (or ??? if blind) and stat uncertainty
    if echo $PAR | grep "blind" > /dev/null; then
        VAL="??"
    else 
        VAL=$(awk "/^$PAR /{print \$2}" $FIT_RESULTS)
    fi
    STAT=$(awk "/^$PAR /{print \$3}" $FIT_RESULTS)
    SYS=$(awk "/^$PAR /{print \$4}" $FIT_RESULTS)

    # Round
    tab_no $VAL $STAT $SYS

}


print_list() {

    # Start list
    NAME=$1
    shift
    OUTFILE="../../../ANA_resources/Tables/Systematics/paper_${NAME}_final_values.tex"
    echo '\begin{center}' > $OUTFILE
    echo '\begin{tabular}{rclclcl}' >> $OUTFILE

    # Fill
    NAMES_FILE="../../names_paper.param"
    for PAR in $*; do
        LATEX=$(grep "^$PAR " $NAMES_FILE | sed "s/^${PAR} //")
        echo "$LATEX &= $(print_one $PAR) \\\\" >> $OUTFILE
    done

    # End
    echo '\end{tabular}' >> $OUTFILE
    echo '\end{center}' >> $OUTFILE

}


# cd ../../../Systematics/ && ./FinalResult && cd -
print_list principal A_signal_KK A_signal_pipi R_signal_KK R_signal_pipi A_signal_pipipipi R_signal_pipipipi R_signal_piK_plus R_signal_piK_minus R_signal_piKpipi_plus R_signal_piKpipi_minus A_signal_Kpi A_signal_Kpipipi
print_list Bs A_Bs_piK A_Bs_piKpipi A_Bs_KK A_Bs_pipi R_Bs_KK R_Bs_pipi A_Bs_pipipipi R_Bs_pipipipi
print_list ADS A_ADS_piK R_ADS_piK A_ADS_piKpipi R_ADS_piKpipi
