#####################################
# Make table with branching fractions
#####################################
OUTFILE="/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Fit_setup/branching_fractions.tex"
INFILE="/home/pullen/analysis/B02DKstar/Parameters/branching_fractions.param"

print_line() {
    echo "\$$1\$ & \$$(awk "/^$2 /{printf \"%s \\\\pm %s\", \$2, \$3}" $INFILE)\$ \\\\" >> $OUTFILE
}

echo '\begin{table}' > $OUTFILE
echo '  \centering' >> $OUTFILE
echo '  \begin{tabular}{cc}' >> $OUTFILE
echo '      \toprule' >> $OUTFILE
echo '      $D$ mode & Branching fraction (\%) \\' >> $OUTFILE
echo '      \midrule' >> $OUTFILE
print_line 'K\pi' Kpi
print_line 'KK' KK
print_line '\pi\pi' pipi
print_line 'K\pi\pi\pi' Kpipipi
print_line '\pi\pi\pi\pi' pipipipi
echo '      \bottomrule' >> $OUTFILE
echo '  \end{tabular}' >> $OUTFILE
echo '  \caption{Branching fractions for the $D$ decays considered in the analysis. These are used to normalise ratios between modes.}' >> $OUTFILE
echo '\label{tab:branching_fractions}' >> $OUTFILE
echo '\end{table}' >> $OUTFILE

