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
    YEAR=2016

    # Start figure
    OUTFILE="../../../ANA_resources/Plots/Monte_carlo/data_vs_MC/reweighting_${MODE}.tex"
    echo '\begin{figure}' > $OUTFILE
    echo '\centering' >> $OUTFILE
    echo '\begin{tabular}{cc}' >> $OUTFILE

    # Fill plots
    echo "$(graphics 'log10(Bd_PT)' '') & $(graphics 'log10(Bd_PT)' 'weight') \\\\" >> $OUTFILE
    echo "\\subfloat[][Unweighted MC]{$(graphics 'Bd_ETA' '')} & \\subfloat[][Weighted MC]{$(graphics 'Bd_ETA' 'weight')} \\\\" >> $OUTFILE

    # End figure
    echo '\end{tabular}' >> $OUTFILE
    echo "\\caption{Comparison of sWeighted $YEAR data (red points) and Monte Carlo (black histogram) in the \$$TEX_NAME\$ mode for the \$B^0$ \$p_\\mathrm{T}$ (above) and \$\\eta$ (below), before and after the MC is reweighted to match the data in these two variables.}"\
        >> $OUTFILE
    echo "\\label{fig:reweighting_$MODE}" >> $OUTFILE
    echo '\end{figure}' >> $OUTFILE
}

process_mode Kpi 'K\pi' 
process_mode Kpipipi 'K\pi\pi\pi' 

