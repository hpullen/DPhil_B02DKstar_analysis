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
    if [[ $MODE != "pipipipi" ]]; then
        cp "${INIT_DIR}/D0_mass_fit_${MODE}_run_1_withCut.pdf" ${PLOT_DIR}
    fi
    cp "${INIT_DIR}/D0_mass_fit_${MODE}_run_2_withCut.pdf" ${PLOT_DIR}
}


# Make each
make_single_figure Kpi 'K\pi'
make_single_figure piK '\pi K'
make_single_figure KK 'KK'
make_single_figure pipi '\pi\pi'
make_single_figure Kpipipi 'K\pi\pi\pi'
make_single_figure piKpipi '\pi K\pi\pi'
make_single_figure pipipipi '\pi\pi\pi\pi'
