# Parse double mis-ID efficiencies into tables

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
    D0_M_EFF_TRUE=$(printf "%.4f" $(awk '/D0_M_true/{print $2}' $INFILE))
    D0_M_ERR_TRUE=$(printf "%.4f" $(awk '/D0_M_true/{print $3}' $INFILE))
    D0_M_EFF_SWAP=$(printf "%.4f" $(awk '/D0_M_swap/{print $2}' $INFILE))
    D0_M_ERR_SWAP=$(printf "%.4f" $(awk '/D0_M_swap/{print $3}' $INFILE))
    D0_M_LINE='$D^0$ window: $\pm$ 25 \mev &'
    D0_M_LINE="${D0_M_LINE} ${D0_M_EFF_TRUE} \$\\pm\$ ${D0_M_ERR_TRUE} &"
    D0_M_LINE="${D0_M_LINE} ${D0_M_EFF_SWAP} \$\\pm\$ ${D0_M_ERR_SWAP} \\\\"
    echo $D0_M_LINE >> $OUTFILE

    # Veto cut efficiency
    VETO_EFF_TRUE=$(printf "%.4f" $(awk '/veto_true/{print $2}' $INFILE))
    VETO_ERR_TRUE=$(printf "%.4f" $(awk '/veto_true/{print $3}' $INFILE))
    VETO_EFF_SWAP=$(printf "%.4f" $(awk '/veto_swap/{print $2}' $INFILE))
    VETO_ERR_SWAP=$(printf "%.4f" $(awk '/veto_swap/{print $3}' $INFILE))
    VETO_LINE='Crossfeed veto: $\pm$ 15 \mev &'
    VETO_LINE="${VETO_LINE} ${VETO_EFF_TRUE} \$\\pm\$ ${VETO_ERR_TRUE} &"
    VETO_LINE="${VETO_LINE} ${VETO_EFF_SWAP} \$\\pm\$ ${VETO_ERR_SWAP} \\\\"
    echo $VETO_LINE >> $OUTFILE

    # Total efficiency
    TOTAL_EFF_TRUE=$(printf "%.4f" $(bc -l <<< "$D0_M_EFF_TRUE * $VETO_EFF_TRUE"))
    TOTAL_ERR_TRUE=$(printf "%.4f" $(bc -l <<< "$TOTAL_EFF_TRUE * sqrt(($D0_M_ERR_TRUE/$D0_M_EFF_TRUE)^2 + ($VETO_ERR_TRUE/$VETO_EFF_TRUE)^2)"))
    TOTAL_EFF_SWAP=$(printf "%.4f" $(bc -l <<< "$D0_M_EFF_SWAP * $VETO_EFF_SWAP"))
    TOTAL_ERR_SWAP=$(printf "%.4f" $(bc -l <<< "$TOTAL_EFF_SWAP * sqrt(($D0_M_ERR_SWAP/$D0_M_EFF_SWAP)^2 + ($VETO_ERR_SWAP/$VETO_EFF_SWAP)^2)"))
    echo '        \midrule' >> $OUTFILE
    echo "        Total & $TOTAL_EFF_TRUE \$\\pm\$ $TOTAL_ERR_TRUE & $TOTAL_EFF_SWAP \$\\pm\$ $TOTAL_ERR_SWAP \\\\" >> $OUTFILE

    # Caption
    echo '        \bottomrule' >> $OUTFILE
    echo '    \end{tabular}' >> $OUTFILE
    if [[ $MODE == "Kpi" ]]; then
        LATEX_MODE='K\pi'
    else 
        LATEX_MODE='K\pi\pi\pi'
    fi
    PROP=$(printf "%.4f" $(bc -l <<< "$TOTAL_EFF_SWAP/$TOTAL_EFF_TRUE"))
    PROP_ERR=$(printf "%.4f" $(bc -l <<< "$PROP * sqrt(($TOTAL_ERR_SWAP/$TOTAL_EFF_SWAP)^2 + ($TOTAL_ERR_TRUE/$TOTAL_EFF_TRUE)^2)"))
    CAPTION="Efficiencies of the \$D^0\$ mass window, crossfeed veto and \$D^0\$ daughter PID cuts for \$B^0 \\to [$LATEX_MODE]_D K^{*0}\$ events in Run ${RUN}. The ratio of expected crossfeed events in the suppressed mode to total events in the favoured mode is $PROP \$\\pm\$ ${PROP_ERR}."
    echo "    \\caption{$CAPTION}" >> $OUTFILE

    # Finish table
    echo "\\label{tab:double_misID_eff_${MODE}_run${RUN}}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE
}


# Make all tables
for CURRENT_MODE in Kpi Kpipipi; do
    for CURRENT_RUN in 1 2; do
        make_table $CURRENT_MODE $CURRENT_RUN
    done
done
