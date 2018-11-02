# Process for one mode
process_mode() {

    # Make plots
    MODE=$1
    TEX_NAME=$2
    CURRENT_DIR=$(pwd)
    OPTIM_DIR="../../../../Optimize_BDT_cut/"
    PLOTS_DIR="../../../../ANA_resources/Plots/Selection/BDT/optimisation/"
    cd $OPTIM_DIR
    root -b -q "PlotFromData.C(\"$MODE\")"
    cd $CURRENT_DIR
    cp "$OPTIM_DIR/Plots/Data/${MODE}_significance_run2.pdf" $PLOTS_DIR
    if [[ $MODE != "pipipipi" ]]; then
        cp "$OPTIM_DIR/Plots/Data/${MODE}_significance_run1.pdf" $PLOTS_DIR
    fi

    # Make figure file
    FIG_FILE="$PLOTS_DIR/${MODE}_plots.tex"
    echo '\begin{figure}' > $FIG_FILE
    echo '\centering' >> $FIG_FILE
    if [[ $MODE != "pipipipi" ]]; then
        echo '\begin{tabular}{cc}' >> $FIG_FILE
        echo "\\subfloat[][Run 1]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Selection/BDT/optimisation/${MODE}_significance_run1.pdf}} & "\
            "\\subfloat[][Run 2]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Selection/BDT/optimisation/${MODE}_significance_run2.pdf}} \\\\" >> $FIG_FILE
        echo '\end{tabular}' >> $FIG_FILE
    else 
        echo "\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Selection/BDT/optimisation/${MODE}_significance_run2.pdf}" >> $FIG_FILE
    fi

    # Caption and label
    echo "\\caption{Plot of predicted significance for different BDT cuts on \$$TEX_NAME\$ data. The cut chosen for the final selection is indicated in red.}" >> $FIG_FILE
    echo "\\label{fig:BDT_sig_$MODE}" >> $FIG_FILE
    echo '\end{figure}' >> $FIG_FILE
}


process_mode Kpi '\pi K'
process_mode KK 'KK'
process_mode pipi '\pi\pi'
process_mode Kpipipi '\pi K\pi\pi'
process_mode pipipipi '4\pi'
