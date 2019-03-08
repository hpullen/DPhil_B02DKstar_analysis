# Script to make .tex file defining data fit plot placement

# Check if combined
if [[ $1 == "--combined" ]]; then
    COMB=true
    EXTRA="_combined"
else 
    COMB=false
    EXTRA=""
fi

# Output file
OUTFILE="/home/pullen/analysis/B02DKstar/ANA_resources/Plots/Data_fit/data_fit_plots_combinedRuns${EXTRA}.tex"
if [[ -f $OUTFILE ]]; then
    rm $OUTFILE
fi


# Function to print one plot
print_plot() {

    # Variables
    MODE=$1
    LATEX_MODE=$2
    if [[ $COMB == true ]]; then
        FORMAT="c"
    else 
        FORMAT="cc"
    fi

echo '\begin{figure}[h]' >> $OUTFILE
echo '    \centering' >> $OUTFILE
echo "    \\begin{tabular}{${FORMAT}}" >> $OUTFILE
if [[ $COMB == true ]]; then
        echo "        \\subfloat[][]{\\includegraphics[width=0.5\\textwidth]{ANA_resources/Plots/Data_fit/twoAndFourBody_data_combinedRuns_${MODE}}}" >> $OUTFILE
else 
        echo "        \\subfloat[][\$B^0 \\to D(${LATEX_MODE})K^{*0}\$]{\\includegraphics[width=0.5\\textwidth]{ANA_resources/Plots/Data_fit/twoAndFourBody_data_split_combinedRuns_${MODE}_plus.pdf}} &" >> $OUTFILE
        echo "        \\subfloat[][\$\\bar{B}^0 \\to D(${LATEX_MODE})\\bar{K}^{*0}\$]{\\includegraphics[width=0.5\\textwidth]{ANA_resources/Plots/Data_fit/twoAndFourBody_data_split_combinedRuns_${MODE}_minus.pdf}} \\\\" >> $OUTFILE
fi
echo '    \end{tabular}' >> $OUTFILE
if [[ $COMB == true ]]; then
    CAP="summed over \$B\$ flavour"
else
    CAP="split by \$B\$ flavour"
fi
echo "    \\caption{Fit to \$B$ invariant mass of selected candidates in the \$${LATEX_MODE}\$ final state, ${CAP}.}" >> $OUTFILE
echo "\\label{fig:data_fit_${MODE}${EXTRA}_combinedRuns}" >> $OUTFILE
echo '\end{figure}' >> $OUTFILE
}


# Print for all modes
print_plot Kpi 'K\pi'
print_plot piK '\pi K'
print_plot KK 'KK'
print_plot pipi '\pi\pi'
print_plot Kpipipi 'K\pi\pi\pi'
print_plot piKpipi '\pi K\pi\pi'
