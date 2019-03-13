# Print value of each observable with statistical and systematic 
source "../s_no.sh"

print_one() {

    # Setup
    PAR=$1
    FIT_RESULTS="../../../Systematics/final_result.param"

    # Names for each run 
    if echo $PAR | grep "piK" > /dev/null; then
        PAR1=$(echo $PAR | sed "s/plus/run1_plus/;s/minus/run1_minus/")
        PAR2=$(echo $PAR | sed "s/plus/run2_plus/;s/minus/run2_minus/")
    else 
        PAR1=${PAR}_run1
        PAR2=${PAR}_run2
    fi

    # Get value (or ??? if blind) and stat uncertainty
    VAL1=$(awk "/^${PAR1} /{printf \"%.3f\", \$2}" $FIT_RESULTS)
    VAL2=$(awk "/^${PAR2} /{printf \"%.3f\", \$2}" $FIT_RESULTS)
    STAT1=$(awk "/^${PAR1} /{printf \"%.3f\", \$3}" $FIT_RESULTS)
    STAT2=$(awk "/^${PAR2} /{printf \"%.3f\", \$3}" $FIT_RESULTS)
    VAL="${VAL1} \$\\pm\$ ${STAT1} & ${VAL2} \$\\pm\$ ${STAT2} \\\\"

    # Print
    echo $VAL

}


print_list() {

    # Start list
    NAME=$1
    shift
    OUTFILE="../../../ANA_resources/Tables/Systematics/${NAME}_values_by_run.tex"
    echo '\begin{tabular}{ccc}' > $OUTFILE
    echo 'Observable & Run 1 & Run 2 \\' >> $OUTFILE
    echo '\midrule' >> $OUTFILE

    # Fill
    NAMES_FILE="../../names.param"
    for PAR in $*; do
        LATEX=$(grep "^$PAR " $NAMES_FILE | sed "s/^${PAR} //")
        echo "$LATEX & $(print_one $PAR)" >> $OUTFILE
        # print_one $PAR
    done

    # End
    echo '\end{tabular}' >> $OUTFILE

}


# cd ../../../Systematics/ && ./FinalResult && cd -
print_list results R_signal_piK_plus R_signal_piK_minus A_signal_KK R_signal_KK R_ds_KK A_signal_pipi R_signal_pipi R_ds_pipi R_signal_piKpipi_plus R_signal_piKpipi_minus
