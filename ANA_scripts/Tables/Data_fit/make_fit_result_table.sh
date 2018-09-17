# Make table from fit result parameters
#!/usr/bin/env bash  
source ../s_no.sh

# Print one line
print_line() {

    # Get parameter name
    PAR=$1
    SHORTPAR=$(echo $PAR | sed 's/pdf_params_//')

    # Get LaTeX form 
    NAMES_FILE="../../names.param"
    if ! grep "^$SHORTPAR " $NAMES_FILE >> "/dev/null"; then
        >&2 echo Error: $SHORTPAR was not found in $NAMES_FILE !
        exit
    fi
    LATEX=$(grep "^$SHORTPAR " $NAMES_FILE | sed "s/^${SHORTPAR} //")

    # Echo ?? if blind
    if echo $SHORTPAR | grep "_blind" >/dev/null; then
        ERR=$(awk "/^$PAR /{printf \"%.3f\", \$3}" raw_fit_result${EXTRA}.param)
        echo "${LATEX} & ?? $\\pm\ ${ERR}$ \\\\"

        # Get value and error 
    else
        VAL=$(n_no $(awk "/^$PAR /{print \$2, \$3}" raw_fit_result${EXTRA}.param))
        echo "${LATEX} & $VAL \\\\"
    fi

}


# See if combined
if [[ $1 == "--combined" ]]; then
    COMB=true
    EXTRA="_combined"
    CAP_EXTRA=" summed over \$B\$ flavour"
else 
    COMB=false
    EXTRA=""
    CAP_EXTRA=""
fi

# Start table
OUTFILE="../../../ANA_resources/Tables/Data_fit/fit_result${EXTRA}.tex"
echo '\begin{table}' > $OUTFILE
echo '  \centering' >> $OUTFILE
echo '  \begin{tabular}{cc}' >> $OUTFILE

# Start left column table
echo '      \begin{tabular}{cc}' >> $OUTFILE
echo '      \toprule' >> $OUTFILE
echo '      Parameter & Value \\' >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# Get number of parameters
N_PARS=$(cat raw_fit_result${EXTRA}.param | wc -l)
if [[ $((N_PARS % 2)) != 0 ]]; then
    ODD=TRUE
else
    ODD=FALSE
fi

# Print all parameters
COUNT=1
SWITCHED=false
for PAR in $(awk '{print $1}' raw_fit_result${EXTRA}.param); do

    # Check if halfway; start new table column
    if [[ $SWITCHED == "false" && $(bc -l <<< "$COUNT >= $N_PARS/2 + 1") == 1 ]]; then
        echo "Count: ${COUNT}. Switching to new column."
        echo '      \bottomrule' >> $OUTFILE
        echo '      \end{tabular}' >> $OUTFILE
        echo '      & \begin{tabular}{cc}' >> $OUTFILE
        echo '      \toprule' >> $OUTFILE
        echo '      Parameter & Value \\' >> $OUTFILE
        echo '      \midrule' >> $OUTFILE
        SWITCHED=true
    fi

    # Print line
    print_line $PAR >> $OUTFILE
    COUNT=$((COUNT + 1))

done

# Add blank line if odd
if [[ $ODD == TRUE ]]; then
    echo ' & \\' >> $OUTFILE
fi

# Finish table
echo '      \bottomrule' >> $OUTFILE
echo '      \end{tabular} \\' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo "  \\caption{Values obtained for floating parameters in the fit to the invariant mass of selected data candidates${CAP_EXTRA}.}" >> $OUTFILE
echo "\\label{tab:fit_result${EXTRA}}" >> $OUTFILE
echo '\end{table}' >> $OUTFILE
