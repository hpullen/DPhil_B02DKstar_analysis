# Print value of each observable with statistical and systematic 
source "../s_no.sh"

print_one() {

    # Setup
    PAR=$1
    PAR_LONG="pdf_params_${PAR}"
    if echo $PAR | grep "R_ds" > /dev/null; then
        FIT_RESULTS="../Data_fit/R_ds_fit_result.param"
    else 
        FIT_RESULTS="../Data_fit/raw_fit_result.param"
    fi

    # Get value (or ??? if blind) and stat uncertainty
    if echo $PAR | grep "_blind" > /dev/null; then
        VAL="??"
        ERR=$(awk "/^$PAR_LONG /{printf \"%.3f\", \$3}" $FIT_RESULTS)
        VAL="${VAL} \$\\pm\$ $ERR"
    else 
        VAL=$(n_no $(awk "/^$PAR_LONG /{print \$2, \$3}" $FIT_RESULTS))
    fi

    # Get sys uncertainty
    SYS_FILE="../../../Systematics/Results/sum.param"
    if grep "${PAR}[\ ]" $SYS_FILE > /dev/null; then
        SYS=$(grep "${PAR}[\ ]" $SYS_FILE | awk '{print $2}')
    else 
        SYS="0"
    fi
    SYS=$(printf '%.3f' $SYS)
    VAL="${VAL} \$\\pm\$ ${SYS}"

    # Print
    echo $VAL

}


print_list() {

    # Start list
    NAME=$1
    shift
    OUTFILE="../../../ANA_resources/Tables/Systematics/${NAME}_final_values.tex"
    echo '\begin{enumerate}' > $OUTFILE

    # Fill
    NAMES_FILE="../../names.param"
    for PAR in $*; do
        LATEX=$(grep "^$PAR " $NAMES_FILE | sed "s/^${PAR} //")
        echo "\\item $LATEX = $(print_one $PAR)" >> $OUTFILE
        # print_one $PAR
    done

    # End
    echo '\end{enumerate}' >> $OUTFILE

}


print_list twoBody A_signal_Kpi A_Bs_piK R_signal_piK_plus_blind R_signal_piK_minus_blind A_signal_KK_run1_blind R_signal_KK_run1_blind A_Bs_KK_run1 R_ds_KK_run1_blind A_signal_pipi_run1_blind R_signal_pipi_run1_blind A_Bs_pipi_run1 R_ds_pipi_run1_blind A_signal_KK_run2_blind R_signal_KK_run2_blind A_Bs_KK_run2 R_ds_KK_run2_blind A_signal_pipi_run2_blind R_signal_pipi_run2_blind A_Bs_pipi_run2 R_ds_pipi_run2_blind
print_list fourBody A_signal_Kpipipi A_Bs_piKpipi R_signal_piKpipi_plus_blind R_signal_piKpipi_minus_blind A_signal_pipipipi_run2_blind R_signal_pipipipi_run2_blind A_Bs_pipipipi_run2 R_ds_pipipipi_run2_blind
