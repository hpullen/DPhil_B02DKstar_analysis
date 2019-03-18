TYPES="branching_ratios selection_efficiency PID production_asymmetry detection_asymmetry signal_shape_pars Bs_low_shape_pars background_shape_pars gamma_pi_inputs DKpipi_inputs charmless"
OPT=$1
if [[ $OPT != "--combineRuns" ]]; then
    TYPES="${TYPES} rho_PID"
fi
for TYPE in $TYPES; do
    echo "Plotting systematics for source: $TYPE"
    ./PlotSystematics $TYPE $OPT
done
