# Run jobs for all types of systematic
# Extra arguments
EXTRAS=$*

# List of systematic types
TYPES="branching_ratios selection_efficiency detection_asymmetry production_asymmetry fixed_rho four_vs_two signal_shape_pars Bs_low_shape_pars background_shape_pars gamma_pi_inputs DKpipi_inputs PID rho_PID pipipipi_low charmless"

# Submit jobs
for TYPE in $TYPES; do
    echo "Submitting jobs for systematic type: $TYPE" 
    ./run_systematic_study.sh 100 $TYPE $EXTRAS
done

