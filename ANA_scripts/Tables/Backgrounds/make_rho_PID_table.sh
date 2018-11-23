#!/usr/bin/env bash

# Output location
OUTFILE="/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/rho_PID.tex"

# Begin table
echo '\begin{table}' > $OUTFILE
echo '    \centering' >> $OUTFILE
echo '    \begin{tabular}{cccc}' >> $OUTFILE
echo '        \toprule' >> $OUTFILE
echo '        & $B^0 \to D\rho^0$ & $B^0 \to DK^{*0}$ & Ratio \\' >> $OUTFILE
echo '        \midrule' >> $OUTFILE

# Source scientific notation 
source ../s_no.sh

# Get PID efficiencies
for RUN in 1 2; do

    # Get PID efficiencies
    EFF_DIR="/home/pullen/analysis/B02DKstar/Efficiencies/Values/"
    PID_SIG_VAL=$(awk "/^Kpi /{print \$2 * 100}" $EFF_DIR/PID_efficiency_run${RUN}.param)
    PID_SIG_ERR=$(awk "/^Kpi /{print \$3 * 100}" $EFF_DIR/PID_efficiency_run${RUN}.param)
    PID_RHO_VAL=$(awk "/^Kpi /{print \$2 * 100}" $EFF_DIR/PID_efficiency_rho_run${RUN}.param)
    PID_RHO_ERR=$(awk "/^Kpi /{print \$3 * 100}" $EFF_DIR/PID_efficiency_rho_run${RUN}.param)

    # Calculate ratio
    RATIO=$(bc -l <<< "$PID_RHO_VAL / $PID_SIG_VAL")
    ERR=$(bc -l <<< "$RATIO * sqrt(($PID_SIG_ERR/$PID_SIG_VAL)^2 + ($PID_RHO_ERR/$PID_RHO_VAL)^2)")
    RATIO_STR=$(s_no $RATIO $ERR)

    # Print line
    echo "Run $RUN & $(n_no $PID_RHO_VAL $PID_RHO_ERR) & $(n_no $PID_SIG_VAL $PID_SIG_ERR) & $RATIO_STR \\\\" >> $OUTFILE

    # Record ratio
    if [[ $RUN == 1 ]]; then
        RATIO1=$RATIO
        ERR1=$ERR
    else 
        RATIO2=$RATIO
        ERR2=$ERR
    fi

done

# Write caption
echo '      \bottomrule' >> $OUTFILE
CAPTION="Comparison of PID efficiencies in \\% between \$B^0 \\to DK^{*0}\$ signal and \$B^0 \\to D\\rho^0\$ background for each run. Efficiencies were calculated using Monte Carlo with PIDCalib. The ratio between the Run 1 and Run 2 signal vs. \$B^0 \\to D\\rho^0\$ ratios is used to scale the Run 2 \$B \\to D\\pi\\pi$ yield with respect to the Run 1 yield in the fit to data."
echo '        \end{tabular}' >> $OUTFILE
echo "        \\caption{$CAPTION}" >> $OUTFILE

# End table
echo "\\label{tab:rho_PID_eff}" >> $OUTFILE
echo '\end{table}' >> $OUTFILE

# Print overall ratio to file
TOT_RATIO=$(bc -l <<< "$RATIO2/$RATIO1")
TOT_ERR=$(bc -l <<< "$RATIO * sqrt(($ERR1/$RATIO1)^2 + ($ERR2/$RATIO2)^2)")
n_no $TOT_RATIO $TOT_ERR > ../../../ANA_resources/Tables/Backgrounds/rho_run_ratio.tex
