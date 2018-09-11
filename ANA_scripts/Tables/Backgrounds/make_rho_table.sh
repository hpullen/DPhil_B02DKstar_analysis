#!/usr/bin/env bash

# Output location
OUTFILE="/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/rho_efficiency.tex"

# Begin table
echo '\begin{table}' > $OUTFILE
echo '    \centering' >> $OUTFILE
echo '    \begin{tabular}{ccccc}' >> $OUTFILE
echo '        \toprule' >> $OUTFILE
echo '        & \multicolumn{2}{c}{Signal} & \multicolumn{2}{c}{$B^0 \to D\rho^0$} \\' >> $OUTFILE
echo '        & Run 1 & Run 2 & Run 1 & Run 2 \\'>> $OUTFILE
echo '        \midrule' >> $OUTFILE

# Source scientific notation 
source ../s_no.sh

# Get branching fractions
BF_FILE="/home/pullen/analysis/B02DKstar/Parameters/branching_fractions_B0.param"
BF_SIGNAL=$(s_no $(awk '/signal/{print $2, $3}' $BF_FILE))
BF_RHO=$(s_no $(awk '/rho/{print $2, $3}' $BF_FILE))
echo "        Branching fraction & \\multicolumn{2}{c}{$BF_SIGNAL} & \\multicolumn{2}{c}{$BF_RHO} \\\\" >> $OUTFILE

# Get acceptance efficiencies
EFF_DIR="/home/pullen/analysis/B02DKstar/Efficiencies/Values/"
ACC_SIG1=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/acceptance_efficiency_run1.param)
ACC_SIG2=$(awk '/Kpi /{printf "$%.1f \\pm %.1f$", $2 * 100, $3 * 100}' $EFF_DIR/acceptance_efficiency_run2.param)
ACC_RHO1=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/acceptance_efficiency_rho_run1.param)
ACC_RHO2=$(awk '/Kpi /{printf "$%.1f \\pm %.1f$", $2 * 100, $3 * 100}' $EFF_DIR/acceptance_efficiency_rho_run2.param)
ACC_SIG1_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/acceptance_efficiency_run1.param)
ACC_SIG1_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/acceptance_efficiency_run1.param)
ACC_SIG2_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/acceptance_efficiency_run2.param)
ACC_SIG2_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/acceptance_efficiency_run2.param)
ACC_RHO1_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/acceptance_efficiency_rho_run1.param)
ACC_RHO1_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/acceptance_efficiency_rho_run1.param)
ACC_RHO2_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/acceptance_efficiency_rho_run2.param)
ACC_RHO2_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/acceptance_efficiency_rho_run2.param)
echo "        Acceptance efficiency (\\%) & $ACC_SIG1 & $ACC_SIG2 & $ACC_RHO1 & $ACC_RHO2 \\\\" >> $OUTFILE

# Get selection efficiencies
SEL_SIG1=$(awk '/Kpi /{printf "$%.3f \\pm %.3f$", $2 * 100, $3 * 100}' $EFF_DIR/selection_efficiency_run1.param)
SEL_SIG2=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/selection_efficiency_run2.param)
SEL_RHO1=$(awk '/Kpi /{printf "$%.3f \\pm %.3f$", $2 * 100, $3 * 100}' $EFF_DIR/selection_efficiency_rho_run1.param)
SEL_RHO2=$(awk '/Kpi /{printf "$%.3f \\pm %.3f$", $2 * 100, $3 * 100}' $EFF_DIR/selection_efficiency_rho_run2.param)
SEL_SIG1_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/selection_efficiency_run1.param)
SEL_SIG1_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/selection_efficiency_run1.param)
SEL_SIG2_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/selection_efficiency_run2.param)
SEL_SIG2_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/selection_efficiency_run2.param)
SEL_RHO1_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/selection_efficiency_rho_run1.param)
SEL_RHO1_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/selection_efficiency_rho_run1.param)
SEL_RHO2_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/selection_efficiency_rho_run2.param)
SEL_RHO2_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/selection_efficiency_rho_run2.param)
echo "        Selection efficiency (\\%) & $SEL_SIG1 & $SEL_SIG2 & $SEL_RHO1 & $SEL_RHO2 \\\\" >> $OUTFILE

# Get PID efficiencies
PID_SIG1=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/PID_efficiency_run1.param)
PID_SIG2=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/PID_efficiency_run2.param)
PID_RHO1=$(awk '/Kpi /{printf "$%.2f \\pm %.2f$", $2 * 100, $3 * 100}' $EFF_DIR/PID_efficiency_rho_run1.param)
PID_RHO2=$(awk '/Kpi /{printf "$%.3f \\pm %.3f$", $2 * 100, $3 * 100}' $EFF_DIR/PID_efficiency_rho_run2.param)
PID_SIG1_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/PID_efficiency_run1.param)
PID_SIG1_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/PID_efficiency_run1.param)
PID_SIG2_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/PID_efficiency_run2.param)
PID_SIG2_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/PID_efficiency_run2.param)
PID_RHO1_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/PID_efficiency_rho_run1.param)
PID_RHO1_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/PID_efficiency_rho_run1.param)
PID_RHO2_VAL=$(awk '/Kpi /{print $2}' $EFF_DIR/PID_efficiency_rho_run2.param)
PID_RHO2_ERR=$(awk '/Kpi /{print $3}' $EFF_DIR/PID_efficiency_rho_run2.param)
echo "        PID efficiency (\\%) & $PID_SIG1 & $PID_SIG2 & $PID_RHO1 & $PID_RHO2 \\\\" >> $OUTFILE

# Calculate total efficiency
TOT_SIG1=$(bc -l <<< "$ACC_SIG1_VAL * $SEL_SIG1_VAL * $PID_SIG1_VAL")
TOT_SIG2=$(bc -l <<< "$ACC_SIG2_VAL * $SEL_SIG2_VAL * $PID_SIG2_VAL")
TOT_RHO1=$(bc -l <<< "$ACC_RHO1_VAL * $SEL_RHO1_VAL * $PID_RHO1_VAL")
TOT_RHO2=$(bc -l <<< "$ACC_RHO2_VAL * $SEL_RHO2_VAL * $PID_RHO2_VAL")
ERR_SIG1=$(bc -l <<< "$TOT_SIG1 * sqrt(($ACC_SIG1_ERR/$ACC_SIG1_VAL)^2 + ($SEL_SIG1_ERR/$SEL_SIG1_VAL)^2 + ($PID_SIG1_ERR/$PID_SIG1_VAL)^2)")
ERR_SIG2=$(bc -l <<< "$TOT_SIG2 * sqrt(($ACC_SIG2_ERR/$ACC_SIG2_VAL)^2 + ($SEL_SIG2_ERR/$SEL_SIG2_VAL)^2 + ($PID_SIG2_ERR/$PID_SIG2_VAL)^2)")
ERR_RHO1=$(bc -l <<< "$TOT_RHO1 * sqrt(($ACC_RHO1_ERR/$ACC_RHO1_VAL)^2 + ($SEL_RHO1_ERR/$SEL_RHO1_VAL)^2)") # + ($PID_RHO1_ERR/$PID_RHO1_VAL)^2)")
ERR_RHO2=$(bc -l <<< "$TOT_RHO2 * sqrt(($ACC_RHO2_ERR/$ACC_RHO2_VAL)^2 + ($SEL_RHO2_ERR/$SEL_RHO2_VAL)^2 + ($PID_RHO2_ERR/$PID_RHO2_VAL)^2)")
TOT_STR_SIG1=$(s_no $TOT_SIG1 $ERR_SIG1) 
TOT_STR_SIG2=$(s_no $TOT_SIG2 $ERR_SIG2)
TOT_STR_RHO1=$(s_no $TOT_RHO1 $ERR_RHO1)
TOT_STR_RHO2=$(s_no $TOT_RHO2 $ERR_RHO2)

# Print total efficiencies
echo '        \midrule' >> $OUTFILE
echo "        Total efficiency & $TOT_STR_SIG1 & $TOT_STR_SIG2 & $TOT_STR_RHO1 & $TOT_STR_RHO2 \\\\" >> $OUTFILE
echo '        \bottomrule' >> $OUTFILE

# Calculate ratios
RATIO1=$(bc -l <<< "$TOT_RHO1/$TOT_SIG1")
RATIO2=$(bc -l <<< "$TOT_RHO2/$TOT_SIG2")
ERR1=$(bc -l <<< "$RATIO1 * sqrt(($ERR_SIG1/$TOT_SIG1)^2 + ($ERR_RHO1/$TOT_RHO1)^2)")
ERR2=$(bc -l <<< "$RATIO2 * sqrt(($ERR_SIG2/$TOT_SIG2)^2 + ($ERR_RHO2/$TOT_RHO2)^2)")
RATIO_STR1=$(s_no $RATIO1 $ERR1)
RATIO_STR2=$(s_no $RATIO2 $ERR2)

# Print ratios to files
echo $RATIO_STR1 > "/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/rho_ratio_run1.tex"
echo $RATIO_STR2 > "/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/rho_ratio_run2.tex"

# Write caption
CAPTION="Comparison of branching fractions and efficiencies between \$B^0 \\to DK^{*0}\$ signal decays and \$B^0 \\to D\\rho^0\$ background. Efficiencies were calculated using Monte Carlo. The ratio of expected \$B^0 \\to D\\rho^0\$ events over signal events is $RATIO_STR1 for Run 1 and $RATIO_STR2 for Run 2."
echo '        \end{tabular}' >> $OUTFILE
echo "        \\caption{$CAPTION}" >> $OUTFILE

# End table
echo '\label{tab:rho_efficiency}' >> $OUTFILE
echo '\end{table}' >> $OUTFILE




