# Table showing BFs/efficiencies for gamma and pi low mass
source ../s_no.sh

# Get single value
get_value() {
    COMP=$1
    FILE=$2
    n_no $(awk "/^$COMP /{print \$2 * 100, \$3 * 100}" $FILE)
}

# Begin table
ANA_DIR="/home/pullen/analysis/B02DKstar/"
TAB_FILE="${ANA_DIR}/ANA_resources/Tables/Backgrounds/gamma_vs_pi.tex"
echo '\begin{table}' > $TAB_FILE
echo '  \centering' >> $TAB_FILE
echo '  \begin{tabular}{ccccc}' >> $TAB_FILE
echo '      \toprule' >> $TAB_FILE
echo '      & \multicolumn{2}{c}{$D^* \to D^0\gamma$} & \multicolumn{2}{c}{$D^* \to D^0\pi^0$} \\' >> $TAB_FILE
echo '      & 010 & 101 & 010 & 101 \\' >> $TAB_FILE
echo '      \midrule' >> $TAB_FILE

# Branching fractions
BF_FILE="${ANA_DIR}/Parameters/branching_fractions_lowMass.param"
echo "Branching fraction (\\%) & \\multicolumn{2}{c}{$(get_value gamma $BF_FILE)} & "\
    "\\multicolumn{2}{c}{$(get_value pi $BF_FILE)} \\\\" >> $TAB_FILE

# Acceptance efficiency
ACC_FILE="${ANA_DIR}/Efficiencies/Values/acceptance_lowMass.param"
echo "Acceptance efficiency (\\%) & $(get_value "gamma_010" $ACC_FILE) & $(get_value "gamma_101" $ACC_FILE)"\
     "& $(get_value "pi_010" $ACC_FILE) & $(get_value "pi_101" $ACC_FILE) \\\\" >> $TAB_FILE

# Selection efficiency
SEL_FILE="${ANA_DIR}/Efficiencies/Values/selection_lowMass_run2.param"
echo "Selection efficiency (\\%) & $(get_value "gamma_010" $SEL_FILE) & $(get_value "gamma_101" $SEL_FILE)"\
     "& $(get_value "pi_010" $SEL_FILE) & $(get_value "pi_101" $SEL_FILE) \\\\" >> $TAB_FILE

# Finish
echo '      \bottomrule' >> $TAB_FILE
echo '  \end{tabular}' >> $TAB_FILE
echo '  \caption{Branching ratios and acceptance and selection efficiencies from partially reconstructed MC samples of $B^0 \to D^*K^{*0}$ for different $D^*$ final states and helicity configurations. These values are used to constrain the relative yields of $D^* \to D^0\gamma$ and $D^* \to D^0\pi^0$ in the fit to data.}' >> $TAB_FILE
echo '\label{tab:gamma_vs_pi}' >> $TAB_FILE
echo '\end{table}' >> $TAB_FILE



# Print G_010 etc to file
# Get one G_010 or P_010 value
get_amount() {

    # Setup
    PARTICLE=$1
    HEL=$2
    COMP=${PARTICLE}_${HEL}

    # Get efficiencies
    BF=$(awk "/^$PARTICLE /{print \$2}" $BF_FILE)
    BF_ERR=$(awk "/^$PARTICLE /{print \$3}" $BF_FILE)
    ACC=$(awk "/^$COMP /{print \$2}" $ACC_FILE)
    ACC_ERR=$(awk "/^$COMP /{print \$3}" $ACC_FILE)
    SEL=$(awk "/^$COMP /{print \$2}" $SEL_FILE)
    SEL_ERR=$(awk "/^$COMP /{print \$3}" $SEL_FILE)
    
    # Multiply
    TOT=$(bc -l <<< "$BF * $ACC * $SEL")
    TOT_ERR=$(bc -l <<< "$TOT * sqrt(($BF_ERR/$BF)^2 + ($ACC_ERR/$ACC)^2 + ($SEL_ERR/$SEL)^2)")
    echo $TOT $TOT_ERR
}

# Get a proportion and uncertainty
get_prop() {

    # Setup
    TOP=$1
    TOP_ERR=$2
    BOTTOM=$3
    BOTTOM_ERR=$4

    # Get fraction
    FRAC=$(bc -l <<< "$TOP / ($TOP + $BOTTOM)")
    FRAC_ERR=$(bc -l <<< "$FRAC * sqrt(($BOTTOM_ERR/$BOTTOM)^2 + 2 * ($TOP_ERR/$TOP)^2)")
    n_no $FRAC $FRAC_ERR
}

# Get each
G_010=$(get_amount gamma 010)
G_101=$(get_amount gamma 101)
P_010=$(get_amount pi 010)
P_101=$(get_amount pi 101)

# Calculate proportions
frac_G_010=$(get_prop $G_010 $P_010)
frac_G_101=$(get_prop $G_101 $P_101)
frac_P_010=$(get_prop $P_010 $G_010)
frac_P_101=$(get_prop $P_101 $G_101)

# Print to file
OUTFILE="${ANA_DIR}/ANA_resources/Tables/Backgrounds/gamma_pi_fractions.tex"
echo '\begin{itemize}' > $OUTFILE
echo '  \item $\frac{G_{010}}{G_{010} + P_{010}} =$ '$frac_G_010',' >> $OUTFILE
echo '  \item $\frac{P_{010}}{G_{010} + P_{010}} =$ '$frac_P_010',' >> $OUTFILE
echo '  \item $\frac{G_{101}}{G_{101} + P_{101}} =$ '$frac_G_101',' >> $OUTFILE
echo '  \item $\frac{P_{101}}{G_{101} + P_{101}} =$ '$frac_P_101'.' >> $OUTFILE
echo '\end{itemize}' >> $OUTFILE
