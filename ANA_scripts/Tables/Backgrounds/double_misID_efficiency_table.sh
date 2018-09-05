# Parse double mis-ID efficiencies into tables

source ../s_no.sh

# Function to make table for a mode and run
make_table() {

    # Get args
    MODE=$1
    RUN=$2

    # Input file to parse
    INFILE="/home/pullen/analysis/B02DKstar/Backgrounds/double_misID/Efficiencies/${MODE}_run${RUN}.param"

    # Output file
    OUTFILE="/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/double_misID/${MODE}_run${RUN}.tex"

    # Name of modes
    if [[ $MODE == "Kpi" ]]; then
        TRUE="\$[K_K \\pi_\\pi]_D\$ efficiency"
        SWAP="\$[K_\\pi \\pi_K]_D\$ efficiency"
    else
        TRUE="\$[K_K \\pi_\\pi \\pi \\pi]_D\$ efficiency"
        SWAP="\$[K_\\pi \\pi_K \\pi \\pi]_D\$ efficiency"
    fi

    # Start table
    echo '\begin{table}' > $OUTFILE
    echo '    \centering' >> $OUTFILE
    echo '    \begin{tabular}{ccc}' >> $OUTFILE
    echo '        \toprule' >> $OUTFILE
    echo "        Selection cut & $TRUE & $SWAP \\\\" >> $OUTFILE
    echo '        \midrule' >> $OUTFILE

    # D0_M cut efficiency
    D0_M_EFF_TRUE=$(awk '/D0_M_true/{print $2}' $INFILE)
    D0_M_ERR_TRUE=$(awk '/D0_M_true/{print $3}' $INFILE)
    D0_M_EFF_SWAP=$(awk '/D0_M_swap/{print $2}' $INFILE)
    D0_M_ERR_SWAP=$(awk '/D0_M_swap/{print $3}' $INFILE)
    D0_M_LINE='$D^0$ window: $\pm$ 25 \mev &'
    D0_M_LINE="${D0_M_LINE} $(printf "%.4f" ${D0_M_EFF_TRUE}) \$\\pm\$ $(printf "%.4f" ${D0_M_ERR_TRUE}) &"
    D0_M_LINE="${D0_M_LINE} $(printf "%.4f" ${D0_M_EFF_SWAP}) \$\\pm\$ $(printf "%.4f" ${D0_M_ERR_SWAP}) \\\\"
    echo $D0_M_LINE >> $OUTFILE

    # Veto cut efficiency
    VETO_EFF_TRUE=$(awk '/veto_true/{print $2}' $INFILE)
    VETO_ERR_TRUE=$(awk '/veto_true/{print $3}' $INFILE)
    VETO_EFF_SWAP=$(awk '/veto_swap/{print $2}' $INFILE)
    VETO_ERR_SWAP=$(awk '/veto_swap/{print $3}' $INFILE)
    VETO_LINE='Crossfeed veto: $\pm$ 15 \mev &'
    VETO_LINE="${VETO_LINE} $(printf "%.4f" ${VETO_EFF_TRUE}) \$\\pm\$ $(printf "%.4f" ${VETO_ERR_TRUE}) &"
    VETO_LINE="${VETO_LINE} $(printf "%.4f" ${VETO_EFF_SWAP}) \$\\pm\$ $(printf "%.4f" ${VETO_ERR_SWAP}) \\\\"
    echo $VETO_LINE >> $OUTFILE

    # PID efficiency
    PID_TRUE_FILE=/home/pullen/analysis/B02DKstar/Efficiencies/Values/PID_efficiency_run${RUN}.param
    PID_SWAP_FILE=/home/pullen/analysis/B02DKstar/Efficiencies/Values/PID_efficiency_doubleSwap_run${RUN}.param
    # PID_EFF_TRUE=$(printf "%.2f" $(awk "/$MODE\ /{print $2}" $PID_TRUE_FILE))
    # PID_EFF_SWAP=$(printf "%.2f" $(awk "/$MODE\ /{print $2}" $PID_SWAP_FILE))
    PID_EFF_TRUE=$(awk "/$MODE\ /{print \$2 / 100}" $PID_TRUE_FILE)
    PID_EFF_SWAP=$(awk "/$MODE\ /{printf \"%.8f\", \$2 / 100}" $PID_SWAP_FILE)
    echo "        PID selection & $(printf "%.4f" $PID_EFF_TRUE) & $(printf "%.4f" $PID_EFF_SWAP) \\\\" >> $OUTFILE

    # Total efficiency
    TOTAL_EFF_TRUE=$(bc -l <<< "$D0_M_EFF_TRUE * $VETO_EFF_TRUE * $PID_EFF_TRUE")
    TOTAL_ERR_TRUE=$(bc -l <<< "$TOTAL_EFF_TRUE * sqrt(($D0_M_ERR_TRUE/$D0_M_EFF_TRUE)^2 + ($VETO_ERR_TRUE/$VETO_EFF_TRUE)^2)")
    TOTAL_EFF_SWAP=$(bc -l <<< "$D0_M_EFF_SWAP * $VETO_EFF_SWAP * $PID_EFF_SWAP")
    TOTAL_ERR_SWAP=$(bc -l <<< "$TOTAL_EFF_SWAP * sqrt(($D0_M_ERR_SWAP/$D0_M_EFF_SWAP)^2 + ($VETO_ERR_SWAP/$VETO_EFF_SWAP)^2)")
    echo '        \midrule' >> $OUTFILE
    TOTAL_LINE="Total & $(printf "%.4f" $TOTAL_EFF_TRUE) \$\\pm\$ $(printf "%.4f" $TOTAL_ERR_TRUE)"
    TOTAL_LINE=${TOTAL_LINE}" & "$(s_no $TOTAL_EFF_SWAP $TOTAL_ERR_SWAP)" \\\\"
    echo $TOTAL_LINE >> $OUTFILE

    # Caption
    echo '        \bottomrule' >> $OUTFILE
    echo '    \end{tabular}' >> $OUTFILE
    if [[ $MODE == "Kpi" ]]; then
        LATEX_MODE='K\pi'
    else 
        LATEX_MODE='K\pi\pi\pi'
    fi
    PROP=$(bc -l <<< "$TOTAL_EFF_SWAP/$TOTAL_EFF_TRUE")
    PROP_ERR=$(bc -l <<< "$PROP * sqrt(($TOTAL_ERR_SWAP/$TOTAL_EFF_SWAP)^2 + ($TOTAL_ERR_TRUE/$TOTAL_EFF_TRUE)^2)")
    CAPTION="Efficiencies of the \$D^0\$ mass window, crossfeed veto and \$D^0\$ daughter PID cuts for \$B^0 \\to [$LATEX_MODE]_D K^{*0}\$ events in Run ${RUN}. The ratio of expected crossfeed events in the suppressed mode to total events in the favoured mode is $(s_no $PROP $PROP_ERR)."
    echo "    \\caption{$CAPTION}" >> $OUTFILE

    # Finish table
    echo "\\label{tab:double_misID_eff_${MODE}_run${RUN}}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE

    # Print efficiency values to files
    echo $VETO_EFF_TRUE | awk '{printf "%.1f", $0 * 100}' > /home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/double_misID/veto_true_${MODE}_run${RUN}.tex
    echo $VETO_EFF_SWAP | awk '{printf "%.1f", $0 * 100}' > /home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/double_misID/veto_swap_${MODE}_run${RUN}.tex

    # Print ratio to files
    s_no $PROP $PROP_ERR > /home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/double_misID/ratio_${MODE}_run${RUN}.tex
}


# Make all tables
for CURRENT_MODE in Kpi Kpipipi; do
    for CURRENT_RUN in 1 2; do
        make_table $CURRENT_MODE $CURRENT_RUN
        echo Made table for $CURRENT_MODE Run $CURRENT_RUN
    done
done
