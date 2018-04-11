#!/usr/bin/env bash 
MC_DIR=$HOME/analysis/B02DKstar/Fit_monte_carlo/Plots
PLOT_DIR=$HOME/analysis/B02DKstar/ANA_resources/Plots/Fit_setup

# Copy plots
cp $MC_DIR/signal_Kpi.pdf $PLOT_DIR
cp $MC_DIR/signal_Kpipipi.pdf $PLOT_DIR
cp $MC_DIR/signal_Bs.pdf $PLOT_DIR

for particle in gamma pi; do
    for hel in 010 101; do
        cp $MC_DIR/lowMass_${particle}_${hel}.pdf $PLOT_DIR
        cp $MC_DIR/lowMass_Bs_${particle}_${hel}.pdf $PLOT_DIR
    done
done

cp $MC_DIR/rho_all_PIDcut_Kpi.pdf $PLOT_DIR
