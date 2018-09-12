#!/usr/bin/env bash
# Script to make figure inputs for charmless background

# Make single figure
make_single_figure() {

    # D mode
    MODE=$1
    LATEX=$2

    # Copy images into correct location
    INIT_DIR="/home/pullen/analysis/B02DKstar/Backgrounds/Charmless/Plots/high_stats/"
    PLOT_DIR="/home/pullen/analysis/B02DKstar/ANA_resources/Plots/Backgrounds/Charmless/"
    for PARTICLE in "B0" "D0"; do
        if [[ $MODE != "pipipipi" ]]; then
            cp "${INIT_DIR}/${PARTICLE}_mass_fit_${MODE}_run_1.pdf" ${PLOT_DIR}
        fi
        cp "${INIT_DIR}/${PARTICLE}_mass_fit_${MODE}_run_2.pdf" ${PLOT_DIR}
    done

    # Make figure .tex file
    FIG_FILE="${PLOT_DIR}/${MODE}_B0_fits.tex"
    echo '\begin{figure}' > $FIG_FILE
    echo '    \centering' >> $FIG_FILE

    # Settings
    TABLE_CONTENT=""
    TABLE_FORMAT="c"
    if [[ $MODE != "pipipipi" ]]; then
        TABLE_FORMAT="cc"
        TABLE_CONTENT="\\subfloat[][Run 1]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Backgrounds/Charmless/B0_mass_fit_${MODE}_run_1}} & "
    fi
    TABLE_CONTENT="${TABLE_CONTENT}\\subfloat[][Run 2]{\\includegraphics[width=0.45\\textwidth]{ANA_resources/Plots/Backgrounds/Charmless/B0_mass_fit_${MODE}_run_2}}"
    TABLE_CONTENT="${TABLE_CONTENT} \\\\"

    # Finish table
    echo "         \\begin{tabular}{${TABLE_FORMAT}}" >> $FIG_FILE        
    echo "             ${TABLE_CONTENT}" >> $FIG_FILE
    echo '         \end{tabular}' >> $FIG_FILE
    echo "    \\caption{Fit to \$B^0\$ invariant mass in the \$D\$ mass sidebands for \$D \\to ${LATEX}\$ with"\
         "a \$D\$ flight distance significance cut of 0 (above) and 3$\\sigma$ (below).}" >> $FIG_FILE
    echo "\\label{fig:B0_charmless_fit_${MODE}}" >> $FIG_FILE
    echo '\end{figure}' >> $FIG_FILE
}


# Make each
make_single_figure Kpi 'K\pi'
make_single_figure piK '\pi K'
make_single_figure KK 'KK'
make_single_figure pipi '\pi\pi'
make_single_figure Kpipipi 'K\pi\pi\pi'
make_single_figure piKpipi '\pi K\pi\pi'
make_single_figure pipipipi '\pi\pi\pi\pi'


