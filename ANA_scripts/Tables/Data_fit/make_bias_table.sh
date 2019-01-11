# Make table with fitter biases for parameters of interest
source ../s_no.sh

for EXTRA in "" "_combinedRuns"; do

    # Start table
    OUTFILE="../../../ANA_resources/Tables/Data_fit/biases${EXTRA}.tex"
    echo '\begin{table}' > $OUTFILE
    echo '  \centering' >> $OUTFILE
    echo '  \small' >> $OUTFILE
    echo '  \begin{tabular}{cc}' >> $OUTFILE
    echo '  \toprule' >> $OUTFILE
    echo '      Parameter & Bias \\' >> $OUTFILE
    echo '  \midrule' >> $OUTFILE

    # Loop through parameters in bias file
    BIAS_FILE="../../../Biases/biases${EXTRA}.param"
    NAME_FILE="../../names.param"
    for PAR in $(awk '{print $1}' $BIAS_FILE); do

        # Get name
        if grep "$PAR " $NAME_FILE >/dev/null; then
            NAME=$(grep "^$PAR " $NAME_FILE | sed "s/^$PAR //")
        else 
            NAME=$(grep "${PAR}_blind" $NAME_FILE | sed "s/^${PAR}_blind //")
        fi

        # Get bias
        BIAS=$(n_no $(awk "/^$PAR /{print \$2, \$3}" $BIAS_FILE))

        # Print
        echo "      $NAME & $BIAS \\\\" >> $OUTFILE
    done

    # Finish table
    echo '      \bottomrule' >> $OUTFILE
    echo '  \end{tabular}' >> $OUTFILE
    echo '  \caption{Biases on the fit parameters of interest.}' >> $OUTFILE
    echo "\\label{tab:biases${EXTRA}}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE

done
