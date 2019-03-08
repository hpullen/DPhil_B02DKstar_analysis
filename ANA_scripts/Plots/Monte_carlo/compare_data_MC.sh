# Assemble data vs. MC plots into a figure
graphics() {
    VAR=$1
    WEIGHT=$2
    echo "\\includegraphics[width=0.3\\textwidth]{ANA_resources/Plots/Monte_carlo/data_vs_MC/$WEIGHT/$MODE/${VAR}_${YEAR}.pdf}"
}

process_mode() {

    # Get mode
    MODE=$1
    TEX_NAME=$2
    YEAR=$3
    WEIGHT=$4
    echo Weight: $WEIGHT
    if [[ $WEIGHT != "weight" ]]; then
        EXTRA="_unweighted"
        CAPTION_EXTRA="before any MC reweighting is applied"
        echo Unweighted! Extra: $EXTRA , Caption: $CAPTION_EXTRA
    else 
        EXTRA=""
        CAPTION_EXTRA="after the MC is reweighted to match the \$B^0\$ \$p_{T}\$ and \$\\eta$ in the data"
        echo Weighted! Extra: $EXTRA , Caption: $CAPTION_EXTRA
    fi

    # Start figure
    OUTFILE="../../../ANA_resources/Plots/Monte_carlo/data_vs_MC/${MODE}_${YEAR}${EXTRA}.tex"
    echo '\begin{figure}' > $OUTFILE
    echo '\centering' >> $OUTFILE
    echo '\begin{tabular}{ccc}' >> $OUTFILE

    # Fill plots
    echo "$(graphics 'acos(Bd_DIRA_OWNPV)' $WEIGHT) & $(graphics 'Bd_IPCHI2_OWNPV' $WEIGHT) & $(graphics 'Bd_LOKI_VFASPF_VCHI2VDOF' $WEIGHT) \\\\" >> $OUTFILE
    echo "$(graphics 'Bd_ptasy_1_50' $WEIGHT) & $(graphics 'log10(D0_IPCHI2_OWNPV)' $WEIGHT) & $(graphics 'log10(KstarK_IPCHI2_OWNPV)' $WEIGHT) \\\\" >> $OUTFILE
    if [[ $MODE == "Kpi" ]]; then
        echo "$(graphics 'log10(D0K_IPCHI2_OWNPV)' $WEIGHT) & $(graphics 'log10(D0Pi_IPCHI2_OWNPV)' $WEIGHT) & $(graphics 'log10(KstarPi_IPCHI2_OWNPV)' $WEIGHT) \\\\" >> $OUTFILE
        echo "$(graphics 'log10(D0K_PT)' $WEIGHT) & $(graphics 'log10(D0Pi_PT)' $WEIGHT) & $(graphics 'log10(KstarK_PT)' $WEIGHT) \\\\" >> $OUTFILE
        echo "$(graphics 'log10(KstarPi_PT)' $WEIGHT) & & \\\\" >> $OUTFILE
    else 
        echo "$(graphics 'log10(KstarPi_IPCHI2_OWNPV)' $WEIGHT) & $(graphics 'log10(KstarK_PT)' $WEIGHT) & $(graphics 'log10(KstarPi_PT)' $WEIGHT) \\\\" >> $OUTFILE
    fi

    # End figure
    echo '\end{tabular}' >> $OUTFILE
    echo "\\caption{Comparison of sWeighted $YEAR data (red points) and selected, weighted Monte Carlo (black histogram) in the \$$TEX_NAME\$ mode for the variables used in the \$$TEX_NAME\$ BDT, ${CAPTION_EXTRA}.}"\
        >> $OUTFILE
    echo "\\label{fig:data_vs_MC_${MODE}_${YEAR}${EXTRA}}" >> $OUTFILE
    echo '\end{figure}' >> $OUTFILE
}

process_mode Kpi 'K\pi' 2016
process_mode Kpi 'K\pi' 2016 weight
process_mode Kpipipi 'K\pi\pi\pi' 2016
process_mode Kpipipi 'K\pi\pi\pi' 2016 weight

