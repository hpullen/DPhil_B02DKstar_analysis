#!/usr/bin/env bash 
# Get ratio between 2-body and 4-body shapes

# Input files
FILE2="../../Fit_monte_carlo/Results/signal_Kpi.param"
FILE4="../../Fit_monte_carlo/Results/signal_Kpipipi.param"

# Get widths
WIDTH2=$(awk '/sigma_L/{print $2}' $FILE2)
ERROR2=$(awk '/sigma_L/{print $3}' $FILE2)
WIDTH4=$(awk '/sigma_L/{print $2}' $FILE4)
ERROR4=$(awk '/sigma_L/{print $3}' $FILE4)

# Calculate ratio and error
RATIO=$(bc -l <<< "$WIDTH4/$WIDTH2")
ERROR=$(bc -l <<< "$RATIO * sqrt(($ERROR2/$WIDTH2)^2 + ($ERROR4/$WIDTH4)^2)")

# Print to parameter file
echo "ratio $RATIO $ERROR" > "../four_body_width_ratio.param"

# Print to tex file
printf '$(%.3f \pm %.3f)$' $RATIO $ERROR > "../../ANA_resources/Tables/Data_fit/width_ratio.tex"
