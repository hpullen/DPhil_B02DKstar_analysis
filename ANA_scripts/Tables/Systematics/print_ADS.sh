# Print value of each observable with statistical and systematic 
source "../s_no.sh"

print_one() {

    # Setup
    PAR=$1
    FIT_RESULTS="../../../Systematics/ADS_results.param"

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
    OUTFILE="../../../ANA_resources/Tables/Systematics/${NAME}_final_values.tex"
    echo '\begin{itemize}' > $OUTFILE

    # Fill
    NAMES_FILE="../../names.param"
    for PAR in $*; do
        LATEX=$(grep "^$PAR " $NAMES_FILE | sed "s/^${PAR} //")
        echo "\\item $LATEX = $(print_one $PAR)" >> $OUTFILE
        # print_one $PAR
    done

    # End
    echo '\end{itemize}' >> $OUTFILE

}


cd ../../../Systematics/ && ./FinalResult && cd -
print_list ADS R_ADS_piK A_ADS_piK R_ADS_piKpipi A_ADS_piKpipi
