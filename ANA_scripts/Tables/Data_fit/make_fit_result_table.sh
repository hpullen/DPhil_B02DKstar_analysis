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


# See if combined runs/combined flavour
# Combined flavour
if [[ $1 == "--combined" ]]; then
    COMB=true
    SPLIT_RUNS=false
    EXTRA="_combinedRuns_combined"
    CAP_EXTRA=" summed over \$B\$ flavour"
    root -b -q 'fit_result_file.C(true, false)'
# Split runs, split flavour
elif [[ $1 == "--splitRuns" ]]; then
    COMB=false
    SPLIT_RUNS=true
    EXTRA=""
    CAP_EXTRA=" split by run"
    root -b -q 'fit_result_file.C(false, true)'
# Default (split flavour, combined runs)
else 
    COMB=false
    SPLIT_RUNS=false
    EXTRA="_combinedRuns"
    CAP_EXTRA=""
    root -b -q 'fit_result_file.C(false, false)'
fi

# Get number of parameters
N_PARS=$(cat raw_fit_result${EXTRA}.param | wc -l)
MAX_ROWS="32"
COLS=$(bc -l <<< "$N_PARS * 1.0/$MAX_ROWS")
if [[ $(bc -l <<< "$COLS > 2.0") == 1 ]]; then
    FORMAT="{ccc}"
else 
    FORMAT="{cc}"
fi

# Start table
OUTFILE="../../../ANA_resources/Tables/Data_fit/fit_result${EXTRA}.tex"
echo '\begin{table}' > $OUTFILE
echo '  \centering' >> $OUTFILE
echo '  \small' >> $OUTFILE
echo "  \\begin{tabular}$FORMAT" >> $OUTFILE

# Start left column table
echo "      \\begin{tabular}$FORMAT" >> $OUTFILE
echo '      \toprule' >> $OUTFILE
echo '      Parameter & Value \\' >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# Print all parameters
COUNT=1
SWITCHED=0
for PAR in $(awk '{print $1}' raw_fit_result${EXTRA}.param); do

    # Check if halfway; start new table column
    if [[ $(bc -l <<< "$COUNT > ($SWITCHED + 1) * $MAX_ROWS") == 1 ]]; then
        echo "Count: ${COUNT}. Switching to new column."
        echo '      \bottomrule' >> $OUTFILE
        echo '      \end{tabular}' >> $OUTFILE
        echo "      & \\begin{tabular}$FORMAT" >> $OUTFILE
        echo '      \toprule' >> $OUTFILE
        echo '      Parameter & Value \\' >> $OUTFILE
        echo '      \midrule' >> $OUTFILE
        SWITCHED=$((SWITCHED + 1))
    fi

    # Print line
    print_line $PAR >> $OUTFILE
    COUNT=$((COUNT + 1))

done

# Add blank lines to fill space
while [[ $(bc -l <<< "$COUNT <= ($SWITCHED + 1) * $MAX_ROWS") == 1 ]]; do
    echo '& \\' >> $OUTFILE
    COUNT=$((COUNT + 1))
done

# Finish table
echo '      \bottomrule' >> $OUTFILE
echo '      \end{tabular} \\' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo "  \\caption{Values obtained for floating parameters in the fit to the invariant mass of selected data candidates${CAP_EXTRA}. \`\`??'' indicates a value that is currently blinded in the fit.}" >> $OUTFILE
echo "\\label{tab:fit_result${EXTRA}}" >> $OUTFILE
echo '\end{table}' >> $OUTFILE
