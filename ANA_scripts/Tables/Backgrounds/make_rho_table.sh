#!/usr/bin/env bash

# Make table for a run
make_run_table() {

    # Run number
    RUN=$1
    RNAME="run"${RUN}

    # Output location
    OUTFILE="/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/rho_efficiency_${RNAME}.tex"

    # Begin table
    echo '\begin{table}' > $OUTFILE
    echo '    \centering' >> $OUTFILE
    echo '    \begin{tabular}{ccc}' >> $OUTFILE
    echo '        \toprule' >> $OUTFILE
    echo '        & Signal & $B^0 \to D\rho^0$ \\' >> $OUTFILE
    echo '        \midrule' >> $OUTFILE

    # Source scientific notation 
    source ../s_no.sh

    # Get branching fractions
    BF_FILE="/home/pullen/analysis/B02DKstar/Parameters/branching_fractions_B0.param"
    BF_SIG=$(s_no $(awk '/signal/{print $2, $3}' $BF_FILE))
    BF_SIG_VAL=$(awk '/signal/{print $2}' $BF_FILE)
    BF_SIG_ERR=$(awk '/signal/{print $3}' $BF_FILE)
    BF_RHO=$(s_no $(awk '/rho/{print $2, $3}' $BF_FILE))
    BF_RHO_VAL=$(awk '/rho/{print $2}' $BF_FILE)
    BF_RHO_ERR=$(awk '/rho/{print $3}' $BF_FILE)
    echo "        Branching fraction & $BF_SIG & $BF_RHO \\\\" >> $OUTFILE

    # Get acceptance efficiencies
    EFF_DIR="/home/pullen/analysis/B02DKstar/Efficiencies/Values/"
    ACC_SIG=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/acceptance_efficiency_${RNAME}.param)
    ACC_RHO=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/acceptance_efficiency_rho_${RNAME}.param)
    ACC_SIG_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/acceptance_efficiency_${RNAME}.param)
    ACC_SIG_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/acceptance_efficiency_${RNAME}.param)
    ACC_RHO_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/acceptance_efficiency_rho_${RNAME}.param)
    ACC_RHO_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/acceptance_efficiency_rho_${RNAME}.param)
    echo "        Acceptance efficiency (\\%) & $ACC_SIG & $ACC_RHO \\\\" >> $OUTFILE

    # Get selection efficiencies
    SEL_SIG=$(awk '/Kpi /{printf "$%.3f \\pm %.3f$", $2 * 100, $3 * 100}' $EFF_DIR/selection_efficiency_${RNAME}.param)
    SEL_RHO=$(awk '/Kpi /{printf "$%.3f \\pm %.3f$", $2 * 100, $3 * 100}' $EFF_DIR/selection_efficiency_rho_${RNAME}.param)
    SEL_SIG_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/selection_efficiency_${RNAME}.param)
    SEL_SIG_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/selection_efficiency_${RNAME}.param)
    SEL_RHO_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/selection_efficiency_rho_${RNAME}.param)
    SEL_RHO_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/selection_efficiency_rho_${RNAME}.param)
    echo "        Selection efficiency (\\%) & $SEL_SIG & $SEL_RHO \\\\" >> $OUTFILE

    # Get PID efficiencies
    PID_SIG=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/PID_efficiency_${RNAME}.param)
    PID_RHO=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/PID_efficiency_rho_${RNAME}.param)
    PID_SIG_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/PID_efficiency_${RNAME}.param)
    PID_SIG_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/PID_efficiency_${RNAME}.param)
    PID_RHO_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/PID_efficiency_rho_${RNAME}.param)
    PID_RHO_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/PID_efficiency_rho_${RNAME}.param)
    echo "        PID efficiency (\\%) & $PID_SIG & $PID_RHO \\\\" >> $OUTFILE

    # Calculate total efficiency
    TOT_SIG=$(bc -l <<< "$BF_SIG_VAL * $ACC_SIG_VAL * $SEL_SIG_VAL * $PID_SIG_VAL")
    TOT_RHO=$(bc -l <<< "$BF_RHO_VAL * $ACC_RHO_VAL * $SEL_RHO_VAL * $PID_RHO_VAL")
    ERR_SIG=$(bc -l <<< "$TOT_SIG * sqrt(($BF_SIG_ERR/$BF_SIG_VAL)^2 + ($ACC_SIG_ERR/$ACC_SIG_VAL)^2 + ($SEL_SIG_ERR/$SEL_SIG_VAL)^2 + ($PID_SIG_ERR/$PID_SIG_VAL)^2)")
    ERR_RHO=$(bc -l <<< "$TOT_RHO * sqrt(($BF_RHO_ERR/$BF_RHO_VAL)^2 + ($ACC_RHO_ERR/$ACC_RHO_VAL)^2 + ($SEL_RHO_ERR/$SEL_RHO_VAL)^2 + ($PID_RHO_ERR/$PID_RHO_VAL)^2)")
    TOT_STR_SIG=$(s_no $TOT_SIG $ERR_SIG)
    TOT_STR_RHO=$(s_no $TOT_RHO $ERR_RHO)

    # Print total efficiencies
    echo '        \midrule' >> $OUTFILE
    echo "        Total & $TOT_STR_SIG & $TOT_STR_RHO \\\\" >> $OUTFILE
    echo '        \bottomrule' >> $OUTFILE

    # Calculate ratios
    RATIO=$(bc -l <<< "$TOT_RHO/$TOT_SIG")
    ERR=$(bc -l <<< "$RATIO * sqrt(($ERR_SIG/$TOT_SIG)^2 + ($ERR_RHO/$TOT_RHO)^2)")
    RATIO_STR=$(s_no $RATIO $ERR)

    # Print ratios to files
    echo $RATIO_STR > "/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/rho_ratio_${RNAME}.tex"

    # Write caption
    CAPTION="Comparison of branching fractions and efficiencies between \$B^0 \\to DK^{*0}\$ signal and \$B^0 \\to D\\rho^0\$ background for Run ${RUN}. Efficiencies were calculated using Monte Carlo. The ratio of expected \$B^0 \\to D\\rho^0\$ events over signal events is $RATIO_STR." 
    echo '        \end{tabular}' >> $OUTFILE
    echo "        \\caption{$CAPTION}" >> $OUTFILE

    # End table
    echo "\\label{tab:rho_efficiency_${RNAME}}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE
}


# Make for each run
make_run_table 1
RATIO_RUN1=$RATIO
ERR_RUN1=$ERR
make_run_table 2
RATIO_RUN2=$RATIO
ERR_RUN2=$ERR

# Print full parameters to parameter file
OUTFILE="../../../Parameters/rho_ratios.param"
echo "run1 $RATIO_RUN1 $ERR_RUN1" > $OUTFILE
echo "run2 $RATIO_RUN2 $ERR_RUN2" >> $OUTFILE
