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
        TRUE="\$[K \\pi]_D\$"
        SWAP="\$[K_\\pi \\pi_K]_D\$"
    else
        TRUE="\$[K \\pi \\pi \\pi]_D\$"
        SWAP="\$[K_\\pi \\pi^\\mathrm{low}_K \\pi \\pi]_D\$"
        SWAP2="\$[K_\\pi \\pi^\\mathrm{high}_K \\pi \\pi]_D\$"
    fi

    # Start table
    echo '\begin{table}' > $OUTFILE
    echo '    \centering' >> $OUTFILE
    FORMAT="ccc"
    if [[ $MODE == "Kpipipi" ]]; then FORMAT="cccc"; fi
    echo "    \\begin{tabular}{${FORMAT}}" >> $OUTFILE
    echo '        \toprule' >> $OUTFILE
    HEADER="Selection cut & $TRUE & $SWAP"
    if [[ $MODE == "Kpipipi" ]]; then
        HEADER="$HEADER & $SWAP2"
    fi
    HEADER="${HEADER} \\\\" 
    echo $HEADER >> $OUTFILE
    echo '        \midrule' >> $OUTFILE

    # D0_M cut efficiency
    # Get values
    D0_M_EFF_TRUE=$(awk '/D0_M_true/{print $2}' $INFILE)
    D0_M_ERR_TRUE=$(awk '/D0_M_true/{print $3}' $INFILE)
    if [[ $MODE == "Kpi" ]]; then
        D0_M_EFF_SWAP=$(awk '/D0_M_swap/{print $2}' $INFILE)
        D0_M_ERR_SWAP=$(awk '/D0_M_swap/{print $3}' $INFILE)
    else 
        D0_M_EFF_SWAP=$(awk '/D0_M_swap_low/{print $2}' $INFILE)
        D0_M_ERR_SWAP=$(awk '/D0_M_swap_low/{print $3}' $INFILE)
        D0_M_EFF_SWAP2=$(awk '/D0_M_swap_high/{print $2}' $INFILE)
        D0_M_ERR_SWAP2=$(awk '/D0_M_swap_high/{print $3}' $INFILE)
    fi

    # Make line for table
    D0_M_LINE='$D^0$ mass window &'
    D0_M_LINE="${D0_M_LINE} $(n_no $D0_M_EFF_TRUE $D0_M_ERR_TRUE) &"
    D0_M_LINE="${D0_M_LINE} $(n_no $D0_M_EFF_SWAP $D0_M_ERR_SWAP) "
    if [[ $MODE == "Kpipipi" ]]; then
        D0_M_LINE="${D0_M_LINE} & $(n_no $D0_M_EFF_SWAP2 $D0_M_ERR_SWAP2) "
    fi
    D0_M_LINE="${D0_M_LINE}\\\\"
    echo $D0_M_LINE >> $OUTFILE

    # Veto cut efficiency
    # Get values
    VETO_EFF_TRUE=$(awk '/veto_true/{print $2}' $INFILE)
    VETO_ERR_TRUE=$(awk '/veto_true/{print $3}' $INFILE)
    if [[ $MODE == "Kpi" ]]; then
        VETO_EFF_SWAP=$(awk '/veto_swap/{print $2}' $INFILE)
        VETO_ERR_SWAP=$(awk '/veto_swap/{print $3}' $INFILE)
    else 
        VETO_EFF_SWAP=$(awk '/veto_swap_low/{print $2}' $INFILE)
        VETO_ERR_SWAP=$(awk '/veto_swap_low/{print $3}' $INFILE)
        VETO_EFF_SWAP2=$(awk '/veto_swap_high/{print $2}' $INFILE)
        VETO_ERR_SWAP2=$(awk '/veto_swap_high/{print $3}' $INFILE)
    fi

    # Make line for table
    if [[ $MODE == "Kpi" ]]; then
        VETO_LINE='Crossfeed veto &'
    else 
        VETO_LINE='Crossfeed vetoes &'
    fi
    VETO_LINE="${VETO_LINE} $(n_no $VETO_EFF_TRUE $VETO_ERR_TRUE) &"
    VETO_LINE="${VETO_LINE} $(n_no $VETO_EFF_SWAP $VETO_ERR_SWAP)"
    if [[ $MODE == "Kpipipi" ]]; then
        VETO_LINE="${VETO_LINE} & $(n_no $VETO_EFF_SWAP2 $VETO_ERR_SWAP2)"
    fi
    VETO_LINE="${VETO_LINE} \\\\"
    echo $VETO_LINE >> $OUTFILE

    # PID efficiency
    # Get values
    PID_TRUE_FILE=/home/pullen/analysis/B02DKstar/Efficiencies/Values/PID_efficiency_run${RUN}.param
    PID_SWAP_FILE=/home/pullen/analysis/B02DKstar/Efficiencies/Values/PID_efficiency_doubleSwap_run${RUN}.param
    PID_EFF_TRUE=$(awk "/$MODE\ /{print \$2}" $PID_TRUE_FILE)
    PID_ERR_TRUE=$(awk "/$MODE\ /{print \$3}" $PID_TRUE_FILE)
    if [[ $MODE == "Kpi" ]]; then
        PID_EFF_SWAP=$(awk "/$MODE\ /{print \$2}" $PID_SWAP_FILE)
        PID_ERR_SWAP=$(awk "/$MODE\ /{print \$3}" $PID_SWAP_FILE)
    else
        PID_EFF_SWAP=$(awk "/${MODE}_low\ /{print \$2}" $PID_SWAP_FILE)
        PID_ERR_SWAP=$(awk "/${MODE}_low\ /{print \$3}" $PID_SWAP_FILE)
        PID_EFF_SWAP2=$(awk "/${MODE}_high\ /{print \$2}" $PID_SWAP_FILE)
        PID_ERR_SWAP2=$(awk "/${MODE}_high\ /{print \$3}" $PID_SWAP_FILE)
    fi

    # Make line
    PID_LINE="PID cuts & $(n_no $PID_EFF_TRUE $PID_ERR_TRUE) & "
    PID_LINE="${PID_LINE} $(n_no $PID_EFF_SWAP $PID_ERR_SWAP)" 
    if [[ $MODE == "Kpipipi" ]]; then
        PID_LINE="${PID_LINE} & $(n_no $PID_EFF_SWAP2 $PID_ERR_SWAP2)" 
    fi
    PID_LINE="${PID_LINE}\\\\"
    echo $PID_LINE >> $OUTFILE

    # Total efficiency
    # True
    TOTAL_EFF_TRUE=$(bc -l <<< "$D0_M_EFF_TRUE * $VETO_EFF_TRUE * $PID_EFF_TRUE")
    TOTAL_ERR_TRUE=$(bc -l <<< "$TOTAL_EFF_TRUE * sqrt(($D0_M_ERR_TRUE/$D0_M_EFF_TRUE)^2 + ($VETO_ERR_TRUE/$VETO_EFF_TRUE)^2 + ($PID_ERR_TRUE/$PID_EFF_TRUE)^2)")

    # Swapped hypothesis
    TOTAL_EFF_SWAP=$(bc -l <<< "$D0_M_EFF_SWAP * $VETO_EFF_SWAP * $PID_EFF_SWAP")
    TOTAL_ERR_SWAP=$(bc -l <<< "$TOTAL_EFF_SWAP * sqrt(($D0_M_ERR_SWAP/$D0_M_EFF_SWAP)^2 + ($VETO_ERR_SWAP/$VETO_EFF_SWAP)^2 + ($PID_ERR_SWAP/$PID_EFF_TRUE)^2)")
    if [[ $MODE == "Kpipipi" ]]; then
        TOTAL_EFF_SWAP2=$(bc -l <<< "$D0_M_EFF_SWAP2 * $VETO_EFF_SWAP2 * $PID_EFF_SWAP2")
        TOTAL_ERR_SWAP2=$(bc -l <<< "$TOTAL_EFF_SWAP2 * sqrt(($D0_M_ERR_SWAP2/$D0_M_EFF_SWAP2)^2 + ($VETO_ERR_SWAP2/$VETO_EFF_SWAP2)^2 + ($PID_ERR_SWAP2/$PID_EFF_TRUE)^2)")
    fi

    # Print to file
    echo '        \midrule' >> $OUTFILE
    TOTAL_LINE="Total & $(s_no $TOTAL_EFF_TRUE $TOTAL_ERR_TRUE)"
    TOTAL_LINE="${TOTAL_LINE} & $(s_no $TOTAL_EFF_SWAP $TOTAL_ERR_SWAP)" 
    if [[ $MODE == "Kpipipi" ]]; then
        TOTAL_LINE="${TOTAL_LINE} & $(s_no $TOTAL_EFF_SWAP2 $TOTAL_ERR_SWAP2)"
    fi
    TOTAL_LINE="${TOTAL_LINE}\\\\"
    echo $TOTAL_LINE >> $OUTFILE

    # End tabular
    echo '        \bottomrule' >> $OUTFILE
    echo '    \end{tabular}' >> $OUTFILE
    if [[ $MODE == "Kpi" ]]; then
        LATEX_MODE='K\pi'
    else
        LATEX_MODE='K\pi\pi\pi'
    fi

    # Calculate ratio
    if [[ $MODE == "Kpi" ]]; then
        PROP=$(bc -l <<< "$TOTAL_EFF_SWAP/$TOTAL_EFF_TRUE")
        PROP_ERR=$(bc -l <<< "$PROP * sqrt(($TOTAL_ERR_SWAP/$TOTAL_EFF_SWAP)^2 + ($TOTAL_ERR_TRUE/$TOTAL_EFF_TRUE)^2)")
    else
        # Low momentum pion
        PROP_LOW=$(bc -l <<< "$TOTAL_EFF_SWAP/$TOTAL_EFF_TRUE")
        PROP_ERR_LOW=$(bc -l <<< "$PROP_LOW * sqrt(($TOTAL_ERR_SWAP/$TOTAL_EFF_SWAP)^2 + ($TOTAL_ERR_TRUE/$TOTAL_EFF_TRUE)^2)")

        # High momentum pion
        PROP_HIGH=$(bc -l <<< "$TOTAL_EFF_SWAP2/$TOTAL_EFF_TRUE")
        PROP_ERR_HIGH=$(bc -l <<< "$PROP_HIGH * sqrt(($TOTAL_ERR_SWAP2/$TOTAL_EFF_SWAP2)^2 + ($TOTAL_ERR_TRUE/$TOTAL_EFF_TRUE)^2)")

        # Sum of ratios
        PROP=$(bc -l <<< "$PROP_LOW + $PROP_HIGH")
        PROP_ERR=$(bc -l <<< "sqrt($PROP_ERR_LOW^2 + $PROP_ERR_HIGH^2)")
    fi

    # Make caption
    if [[ $MODE == "Kpi" ]]; then
        CAPTION="Efficiencies of the \$\\pm 25\$\\mev \$D^0\$ mass window, \$\\pm 15\$\\mev crossfeed veto and PID cuts for \$B^0 \\to [$LATEX_MODE]_D K^{*0}\$ events in Run ${RUN}. The ratio of expected crossfeed events in the suppressed mode to total events in the favoured mode is $(s_no $PROP $PROP_ERR)."
    else
        CAPTION="Efficiencies of the \$\\pm 25\$\\mev \$D^0\$ mass window, \$\\pm 15\$\\mev crossfeed vetoes and PID cuts for \$B^0 \\to [$LATEX_MODE]_D K^{*0}\$ events in Run ${RUN}. Since the kaon can be swapped with either of the opposite-sign pions, the efficiencies are calculated for both cases. The ratio of expected crossfeed events in the suppressed mode to total events in the favoured mode is $(s_no $PROP_LOW $PROP_ERR_LOW) where the lower momentum pion is swapped with the kaon, and $(s_no $PROP_HIGH $PROP_ERR_HIGH) where the higher momentum pion is swapped."
    fi
    echo "    \\caption{$CAPTION}" >> $OUTFILE

    # Finish table
    echo "\\label{tab:double_misID_eff_${MODE}_run${RUN}}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE

    # Print efficiency values to files
    echo $VETO_EFF_TRUE | awk '{printf "%.1f", $0 * 100}' > /home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/double_misID/veto_true_${MODE}_run${RUN}.tex
    echo $VETO_EFF_SWAP | awk '{printf "%.1f", $0 * 100}' > /home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/double_misID/veto_swap_${MODE}_run${RUN}.tex

    # Print ratio to files
    s_no $PROP $PROP_ERR > /home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/double_misID/ratio_${MODE}_run${RUN}.tex
    if [[ $MODE == "Kpipipi" ]]; then
        s_no $PROP_LOW $PROP_ERR_LOW > /home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/double_misID/ratio_${MODE}_low_run${RUN}.tex
        s_no $PROP_HIGH $PROP_ERR_HIGH > /home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/double_misID/ratio_${MODE}_high_run${RUN}.tex
    fi

    # Print ratio to parameter files
    echo "$PROP $PROP_ERR" > /home/pullen/analysis/B02DKstar/Parameters/double_misID/ratio_${MODE}_run${RUN}.param
}


# Make all tables
for CURRENT_MODE in Kpi Kpipipi; do
    for CURRENT_RUN in 1 2; do
        make_table $CURRENT_MODE $CURRENT_RUN
        echo Made table for $CURRENT_MODE Run $CURRENT_RUN
    done
done
