#!/usr/bin/env bash 
FIT_DIR=$HOME/analysis/B02DKstar/Fit_data/Plots
PLOT_DIR=$HOME/analysis/B02DKstar/ANA_resources/Plots/Data_fit
if [[ $1 == "--combined" ]]; then
    COMB=true
else 
    COMB=false
fi

for mode in Kpi piK KK pipi Kpipipi piKpipi pipipipi; do
    for run_num in 1 2; do
        if [[ $COMB == true ]]; then
            if [[ ! ($run_num == "1" && $mode == "pipipipi") ]]; then
                cp $FIT_DIR/twoAndFourBody_data_${mode}_run${run_num}.pdf $PLOT_DIR
            fi
        else 
            for sign in plus minus; do
                if [[ ! ($run_num == "1" && $mode == "pipipipi") ]]; then
                    cp $FIT_DIR/twoAndFourBody_data_split_${mode}_run${run_num}_${sign}.pdf $PLOT_DIR
                fi
            done
        fi
    done
done

