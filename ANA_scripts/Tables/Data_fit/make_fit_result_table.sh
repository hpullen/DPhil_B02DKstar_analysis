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

    # Get value and error 
    VAL=$(n_no $(awk "/^$PAR /{print \$2, \$3}" raw_fit_result.param))
    echo "${LATEX} & $VAL \\\\"

}


# Start table
OUTFILE="../../../ANA_resources/Tables/Data_fit/fit_result.tex"
echo '\begin{table}' > $OUTFILE
echo '  \centering' >> $OUTFILE
echo '  \begin{tabular}{cc}' >> $OUTFILE

# Start left column table
echo '      \begin{tabular}{cc}' >> $OUTFILE
echo '      \toprule' >> $OUTFILE
echo '      Parameter & Value \\' >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# Get number of parameters
N_PARS=$(cat raw_fit_result.param | wc -l)

# Print all parameters
COUNT=1
SWITCHED=false
for PAR in $(awk '{print $1}' raw_fit_result.param); do

    # Check if halfway; start new table column
    if [[ $SWITCHED == "false" && $(bc -l <<< "$COUNT > $N_PARS/2") == 1 ]]; then
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

# Finish table
echo '      \bottomrule' >> $OUTFILE
echo '      \end{tabular} \\' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo '  \caption{Values obtained for floating parameters in the fit to the invariant mass of selected data candidates.}' >> $OUTFILE
echo '\label{tab:fit_result}' >> $OUTFILE
echo '\end{table}' >> $OUTFILE
