# Assemble data vs. MC plots into a figure
graphics() {
    VAR=$1
    echo "\\includegraphics[width=0.3\\textwidth]{ANA_resources/Plots/Monte_carlo/data_vs_MC/weight/$MODE/${VAR}_${YEAR}.pdf}"
}

process_mode() {

    # Get mode
    MODE=$1
    TEX_NAME=$2
    YEAR=$3

    # Start figure
    OUTFILE="../../../ANA_resources/Plots/Monte_carlo/data_vs_MC/${MODE}_${YEAR}.tex"
    echo '\begin{figure}' > $OUTFILE
    echo '\centering' >> $OUTFILE
    echo '\begin{tabular}{ccc}' >> $OUTFILE

    # Fill plots
    echo "$(graphics 'acos(Bd_DIRA_OWNPV)') & $(graphics 'Bd_IPCHI2_OWNPV') & $(graphics 'Bd_LOKI_VFASPF_VCHI2VDOF') \\\\" >> $OUTFILE
    echo "$(graphics 'Bd_ptasy_1_50') & $(graphics 'log10(D0_IPCHI2_OWNPV)') & $(graphics 'log10(KstarK_IPCHI2_OWNPV)') \\\\" >> $OUTFILE
    if [[ $MODE == "Kpi" ]]; then
        echo "$(graphics 'log10(D0K_IPCHI2_OWNPV)') & $(graphics 'log10(D0Pi_IPCHI2_OWNPV)') & $(graphics 'log10(KstarPi_IPCHI2_OWNPV)') \\\\" >> $OUTFILE
        echo "$(graphics 'log10(D0K_PT)') & $(graphics 'log10(D0Pi_PT)') & $(graphics 'log10(KstarK_PT)') \\\\" >> $OUTFILE
        echo "$(graphics 'log10(KstarPi_PT)') & & \\\\" >> $OUTFILE
    else 
        echo "$(graphics 'log10(KstarPi_IPCHI2_OWNPV)') & $(graphics 'log10(KstarK_PT)') & $(graphics 'log10(KstarPi_PT)') \\\\" >> $OUTFILE
    fi

    # End figure
    echo '\end{tabular}' >> $OUTFILE
    echo "\\caption{Comparison of sWeighted $YEAR data (red points) and selected, weighted Monte Carlo (black histogram) in the \$$TEX_NAME\$ mode for the variables used in the \$$TEX_NAME\$ BDT.}"\
        >> $OUTFILE
    echo "\\label{fig:data_vs_MC_${MODE}_${YEAR}}" >> $OUTFILE
    echo '\end{figure}' >> $OUTFILE
}

process_mode Kpi 'K\pi' 2012
process_mode Kpi 'K\pi' 2016
process_mode Kpipipi 'K\pi\pi\pi' 2012
process_mode Kpipipi 'K\pi\pi\pi' 2016

