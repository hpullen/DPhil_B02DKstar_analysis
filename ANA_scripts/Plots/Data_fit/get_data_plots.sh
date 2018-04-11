#!/usr/bin/env bash 
FIT_DIR=$HOME/analysis/B02DKstar/Fit_data_new/Plots
PLOT_DIR=$HOME/analysis/B02DKstar/ANA_resources/Plots/Data_fit

for mode in Kpi piK KK pipi Kpipipi piKpipi pipipipi; do
    for sign in plus minus; do
        cp $FIT_DIR/twoAndFourBody_data_split_${mode}_${sign}.pdf $PLOT_DIR
    done
done

