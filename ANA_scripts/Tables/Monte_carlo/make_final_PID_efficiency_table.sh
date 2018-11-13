# Make table of PID efficiency and alternate efficiency with difference
source ../s_no.sh

# Table for one mode
make_table() {

    # Get mode
    MODE=$1
    TEX_NAME=$2

    # Begin table
    OUTFILE="../../../ANA_resources/Tables/Monte_carlo/PID/PID_final_${MODE}.tex"
    echo '\begin{table}' > $OUTFILE
    echo '    \centering' >> $OUTFILE
    if [[ $MODE == "pipipipi" ]]; then
        FORMAT="ccc"
    else
        FORMAT="ccccc"
    fi
    echo "    \\begin{tabular}{$FORMAT}" >> $OUTFILE
    HEADER1=''
    HEADER2='& $B^0$ & $\bar{B}^0$'
    if [[ $MODE != "pipipipi" ]]; then
        HEADER1=${HEADER1}' & \multicolumn{2}{c}{Run 1}'
        HEADER2=${HEADER2}' & $B^0$ & $\bar{B}^0$'
    fi
    HEADER1=${HEADER1}' & \multicolumn{2}{c}{Run 2}\\'
    HEADER2=${HEADER2}'\\'
    echo '    \toprule' >> $OUTFILE
    echo $HEADER1 >> $OUTFILE
    echo $HEADER2 >> $OUTFILE
    echo '    \midrule' >> $OUTFILE

    # Fill table
    LINE="Default binning"
    ALT_LINE="Finer binning"
    DIFF_LINE="Difference"
    CATS=""
    if [[ $MODE != "pipipipi" ]]; then
        CATS="B0_run1 B0bar_run1 "
    fi
    CATS=${CATS}"B0_run2 B0bar_run2"
    for CAT in $CATS; do
        FINAL_FILE="../../../Efficiencies/Values/PID_efficiency_${CAT}_final.param"
        ALT_FILE="../../../Efficiencies/Values/PID_efficiency_${CAT}_alt.param"
        VAL=$(awk "/^$MODE /{printf \"%.2f\", \$2 * 100}" $FINAL_FILE) 
        ALT=$(awk "/^$MODE /{printf \"%.2f\", \$2 * 100}" $ALT_FILE) 
        DIFF=$(awk "/^$MODE /{printf \"%.2f\", \$3 * 100}" $FINAL_FILE) 
        LINE=${LINE}" & $VAL"
        ALT_LINE=${ALT_LINE}" & $ALT"
        DIFF_LINE=${DIFF_LINE}" & $DIFF"
    done
    echo "${LINE}\\\\" >> $OUTFILE
    echo "${ALT_LINE}\\\\" >> $OUTFILE
    echo "${DIFF_LINE}\\\\" >> $OUTFILE

    # Finish table
    echo '    \bottomrule' >> $OUTFILE
    echo '    \end{tabular}' >> $OUTFILE
    echo "    \\caption{PID efficiencies in \\% for \$$TEX_NAME\$ evaluated with the default PIDCalib binning scheme and a finer binning scheme. The difference in these values is used as the systematic uncertainty.}" >> $OUTFILE
    echo "\\label{tab:final_PID_$MODE}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE


}

make_table Kpi 'K\pi'
make_table KK 'KK'
make_table pipi '\pi\pi'
make_table Kpipipi 'K3\pi'
make_table pipipipi '4\pi'
