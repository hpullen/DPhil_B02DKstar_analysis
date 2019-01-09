#!/usr/bin/env bash 
FIT_DIR=$HOME/analysis/B02DKstar/Fit_data/Plots
PLOT_DIR=$HOME/analysis/B02DKstar/ANA_resources/Plots/Data_fit

for mode in Kpi piK KK pipi Kpipipi piKpipi pipipipi; do
    for run_num in 1 2; do

        # Copy flavour combined fit plots
            if [[ ! ($run_num == "1" && $mode == "pipipipi") ]]; then
                cp $FIT_DIR/twoAndFourBody_data_${mode}_run${run_num}.pdf $PLOT_DIR
            fi

        # Copy flavour split plots
        for sign in plus minus; do
            if [[ ! ($run_num == "1" && $mode == "pipipipi") ]]; then
                cp $FIT_DIR/twoAndFourBody_data_split_${mode}_run${run_num}_${sign}.pdf $PLOT_DIR
            fi
        done
    done

    # Copy run combined plots
    # Flavour combined fit plots
    if [[ $mode != "pipipipi" ]]; then
        cp $FIT_DIR/twoAndFourBody_data_combinedRuns_${mode}.pdf $PLOT_DIR

        # Flavour split plots
        for sign in plus minus; do
            cp $FIT_DIR/twoAndFourBody_data_split_combinedRuns_${mode}_${sign}.pdf $PLOT_DIR
        done
    fi
done

