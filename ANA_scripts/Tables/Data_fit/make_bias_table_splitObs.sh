# Make table with fitter biases for parameters of interest
source ../s_no.sh

for EXTRA in ""; do

    # Start table
    OUTFILE="../../../ANA_resources/Tables/Data_fit/biases_splitObs.tex"
    echo '\begin{table}' > $OUTFILE
    echo '  \centering' >> $OUTFILE
    echo '  \small' >> $OUTFILE
    echo '  \begin{tabular}{ccc}' >> $OUTFILE
    echo '  \toprule' >> $OUTFILE
    echo '      Parameter & Run 1 bias & Run 2 bias \\' >> $OUTFILE
    echo '  \midrule' >> $OUTFILE

    # Loop through parameters in bias file
    BIAS_FILE="../../../Biases/biases_splitObs.param"
    NAME_FILE="../../names.param"
    for PAR in $(awk '{print $1}' $BIAS_FILE); do

        # Get name and bias
        PAR_SHORT=$(sed 's/_run1//; s/_run2//' <<< $PAR)
        NAME=$(grep "^$PAR_SHORT " $NAME_FILE | sed "s/^$PAR_SHORT //")
        BIAS=$(n_no $(awk "/^$PAR /{print \$2, \$3}" $BIAS_FILE))

        # Check for 4pi: no run 1 value
        if grep "pipipipi" <<< $PAR >/dev/null; then
            echo "$NAME & -- & $BIAS \\\\" >> $OUTFILE
        else 
            if grep "run1" <<< $PAR >/dev/null; then
                PAR2=$(sed "s/run1/run2/" <<< $PAR)
                BIAS2=$(n_no $(awk "/^$PAR2 /{print \$2, \$3}" $BIAS_FILE))
                echo "$NAME & $BIAS & $BIAS2 \\\\" >> $OUTFILE
            fi
        fi
    done

    # Finish table
    echo '      \bottomrule' >> $OUTFILE
    echo '  \end{tabular}' >> $OUTFILE
    echo '  \caption{Biases on the fit parameters of interest, split by run.}' >> $OUTFILE
    echo "\\label{tab:biases_splitObs}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE

done
