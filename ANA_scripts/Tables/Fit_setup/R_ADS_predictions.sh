# Print predicted values of R_ADS plus and minus to files.
source ../s_no.sh
print_one() {
    SIGN=$1
    EXTRA=$2
    INFILE="../../../Parameters/predicted_observables.param"
    n_no $(awk "/^R_${SIGN}${EXTRA} /{print \$2, \$3}" $INFILE) 
}

OUTFILE="../../../ANA_resources/Tables/Fit_setup/Values/R_plus_minus_values.tex"
echo '\begin{itemize}' > $OUTFILE
echo '\item $\mathcal{R}^+_{\pi K} =$ '$(print_one plus) >> $OUTFILE
echo '\item $\mathcal{R}^-_{\pi K} =$ '$(print_one minus)            >> $OUTFILE
echo '\item $\mathcal{R}^+_{\pi K\pi\pi} =$ '$(print_one plus _K3pi) >> $OUTFILE
echo '\item $\mathcal{R}^-_{\pi K\pi\pi} =$ '$(print_one minus _K3pi)>> $OUTFILE
echo '\end{itemize}' >> $OUTFILE
