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
OUTFILE="/home/pullen/analysis/B02DKstar/ANA_resources/Plots/Data_fit/data_fit_plots${EXTRA}.tex"
if [[ -f $OUTFILE ]]; then
    rm $OUTFILE
fi


# Function to print one plot
print_plot() {

    # Variables
    MODE=$1
    LATEX_MODE=$2
    if [[ $MODE == "pipipipipi" && $COMB == true ]]; then
        FORMAT="c"
    else 
        FORMAT="cc"
    fi

echo '\begin{figure}[h]' >> $OUTFILE
echo '    \centering' >> $OUTFILE
echo "    \\begin{tabular}{${FORMAT}}" >> $OUTFILE
if [[ $COMB == true ]]; then
    if [[ $MODE != "pipipipi" ]]; then 
        echo "        \\subfloat[][\$B^0 \\to D(${LATEX_MODE})K^{*0}\$ Run 1]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Data_fit/twoAndFourBody_data_${MODE}_run1}} &" >> $OUTFILE
    fi
    echo "        \\subfloat[][\$B^0 \\to D(${LATEX_MODE})K^{*0}\$ Run 2]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Data_fit/twoAndFourBody_data_${MODE}_run2.pdf}} \\\\" >> $OUTFILE
else 
    if [[ $MODE != "pipipipi" ]]; then 
        echo "        \\subfloat[][\$B^0 \\to D(${LATEX_MODE})K^{*0}\$ Run 1]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Data_fit/twoAndFourBody_data_split_${MODE}_run1_plus.pdf}} &" >> $OUTFILE
        echo "        \\subfloat[][$\\bar{B}^0 \\to D(${LATEX_MODE})\\bar{K}^{*0}\$ Run 1]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Data_fit/twoAndFourBody_data_split_${MODE}_run1_minus.pdf}} \\\\" >> $OUTFILE
    fi
    echo "        \\subfloat[][\$B^0 \\to D(${LATEX_MODE})K^{*0}\$ Run 2]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Data_fit/twoAndFourBody_data_split_${MODE}_run2_plus.pdf}} &" >> $OUTFILE
    echo "        \\subfloat[][$\\bar{B}^0 \\to D(${LATEX_MODE})\\bar{K}^{*0}\$ Run 2]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Data_fit/twoAndFourBody_data_split_${MODE}_run2_minus.pdf}} \\\\" >> $OUTFILE
fi
echo '    \end{tabular}' >> $OUTFILE
if [[ $COMB == true ]]; then
    CAP="split by run and summed over \$B\$ flavour"
else
    CAP="split by \$B\$ flavour and run"
fi
echo "    \\caption{Fit to \$B$ invariant mass of selected candidates in the \$${LATEX_MODE}\$ final state, ${CAP}.}" >> $OUTFILE
echo "\\label{fig:data_fit_${MODE}${EXTRA}}" >> $OUTFILE
echo '\end{figure}' >> $OUTFILE
}


# Print for all modes
print_plot Kpi 'K\pi'
print_plot piK '\pi K'
print_plot KK 'KK'
print_plot pipi '\pi\pi'
print_plot Kpipipi 'K\pi\pi\pi'
print_plot piKpipi '\pi K\pi\pi'
print_plot pipipipi '\pi\pi\pi\pi'
