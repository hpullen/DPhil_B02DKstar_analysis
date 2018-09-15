MODE=$1
cd /home/pullen/analysis/B02DKstar/Fit_data/
./Significance 1 $MODE
./PlotFit Histograms/toy_twoAndFourBody_signal.root Plots/toy_zero${MODE}_signal N
./PlotFit Histograms/toy_twoAndFourBody_null.root Plots/toy_zero${MODE}_null N
