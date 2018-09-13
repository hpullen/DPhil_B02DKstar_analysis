# Make tables for low mass fit parameters
source ../s_no.sh

# Get value for one component/parameter
get_value() {
    COMP=$1
    PAR=$2
    INFILE="/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/lowMass_${COMP}.param"
    n_no $(awk "/^$PAR /{print \$2, \$3}" $INFILE)
}

# Print one line
print_line() {
    PAR=$1
    LATEX=$2
    PARENT=$3
    if [[ $PARENT == "Bs" ]]; then
        STR="Bs_"
    else
        STR=""
    fi
    echo "\$$LATEX\$ & $(get_value ${STR}gamma_010 $PAR) & $(get_value ${STR}gamma_101 $PAR) & $(get_value ${STR}pi_010 $PAR) & $(get_value ${STR}pi_101 $PAR) \\\\"
}

# Make Bs or B0 table
make_single_table() {

    # Setup
    PARENT=$1
    if [[ $PARENT == "Bs" ]]; then
        DECAY='$B_s^0 \to D^*K^{*0}$'
        EXTRA="_Bs"
    else
        DECAY='$B^0 \to D^*K^{*0}$'
        EXTRA=""
    fi
    TAB_FILE="/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Fit_setup/low_mass_params${EXTRA}.tex"

    # Begin table
    echo '\begin{table}' > $TAB_FILE
    echo '  \centering' >> $TAB_FILE
    echo '  \begin{tabular}{ccccc}' >> $TAB_FILE
    echo '      \toprule' >> $TAB_FILE
    echo '      & \multicolumn{2}{c}{$D^* \to D^0\gamma$} & \multicolumn{2}{c}{$D^* \to D^0\pi^0$} \\' >> $TAB_FILE
    echo '      & 010 & 101 & 010 & 101 \\' >> $TAB_FILE
    echo '      \midrule' >> $TAB_FILE

    # Fill in parameters
    print_line a 'a' $PARENT >> $TAB_FILE
    print_line b 'b' $PARENT >> $TAB_FILE
    print_line sigma '\sigma' $PARENT >> $TAB_FILE
    print_line ratio 'R_\sigma' $PARENT >> $TAB_FILE
    print_line frac 'f' $PARENT >> $TAB_FILE
    print_line csi '\xi' $PARENT >> $TAB_FILE

    # End table
    echo '      \bottomrule' >> $TAB_FILE
    echo '  \end{tabular}' >> $TAB_FILE
    echo "  \\caption{Parameters obtained from fit to $DECAY MC.}" >> $TAB_FILE
    echo "\\label{tab:part_reco_parameters${EXTRA}}" >> $TAB_FILE
    echo '\end{table}' >> $TAB_FILE

}

make_single_table
make_single_table Bs


