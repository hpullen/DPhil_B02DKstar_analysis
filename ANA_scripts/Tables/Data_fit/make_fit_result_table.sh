# Make table from fit result parameters
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
echo '      \toprule' >> $OUTFILE
echo '      Parameter & Value \\' >> $OUTFILE
echo '      \midrule' >> $OUTFILE

# Print all parameters
for PAR in $(awk '{print $1}' raw_fit_result.param); do
    print_line $PAR >> $OUTFILE
done

# Finish table
echo '      \bottomrule' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo '  \caption{Values obtained for floating parameters in the fit to the invariant mass of selected data candidates.}' >> $OUTFILE
echo '\label{tab:fit_result}' >> $OUTFILE
echo '\end{table}' >> $OUTFILE
