###########################################################################
# Make table containing Monte Carlo fit parameters for Kpi and Kpipipi fits
###########################################################################
# Location of files with parameters and table
PARAM_FILE2="/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/signal_Kpi.param"
PARAM_FILE4="/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/signal_Kpipipi.param"
TAB_FILE="/home/pullen/analysis/B02DKstar/ANA_tables/MC_signal_fit.tex"

# Remove table if it already exists
if [ -f $TAB_FILE ]; then
    rm $TAB_FILE
fi

# Print LaTeX table to file
echo '\documentclass{article}'            >> $TAB_FILE
echo '\begin{document}'                   >> $TAB_FILE
echo '\begin{table}[H]'                   >> $TAB_FILE
echo '    \centering'                     >> $TAB_FILE
echo '    \begin{tabular}{ccc}'           >> $TAB_FILE
echo '        \toprule'                   >> $TAB_FILE
echo '        & $K\pi$ & $K\pi\pi\pi$ \\' >> $TAB_FILE
echo '        \midrule'                   >> $TAB_FILE
echo "        $\\mu$ & "\
             "$(grep mean $PARAM_FILE2 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "& $(grep mean $PARAM_FILE4 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "\\\\"                       >> $TAB_FILE
echo "        $\\sigma_L$ &"\
             "$(grep sigma_L $PARAM_FILE2 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "& $(grep sigma_L $PARAM_FILE4 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "\\\\"                       >> $TAB_FILE
echo "        \$R_{\\sigma}$ &"\
             "$(grep sigma_ratio $PARAM_FILE2 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "& $(grep sigma_ratio $PARAM_FILE4 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "\\\\"                       >> $TAB_FILE
echo "        $\\alpha_L$ &"\
             "$(grep alpha_L $PARAM_FILE2 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "& $(grep alpha_L $PARAM_FILE4 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "\\\\"                       >> $TAB_FILE
echo "        $\\alpha_R$ &"\
             "$(grep alpha_R $PARAM_FILE2 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "& $(grep alpha_R $PARAM_FILE4 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "\\\\"                       >> $TAB_FILE
echo "        \$n_L$ &"\
             "$(grep n_L $PARAM_FILE2 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "& $(grep n_L $PARAM_FILE4 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "\\\\"                       >> $TAB_FILE
echo "        \$f_{cb}$ &"\
             "$(grep frac $PARAM_FILE2 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "& $(grep frac $PARAM_FILE4 | awk '{printf "$%.2f \\pm %.2f$", $2, $3}')"\
             "\\\\"                       >> $TAB_FILE
echo '       \bottomrule'               >> $TAB_FILE
echo '       \end{tabular}'               >> $TAB_FILE
echo '    \caption{Signal shape parameters obtained from a fit to Run 2 Monte'\
         'Carlo for both $K\pi$ and $K\pi\pi\pi$. Parameter names are defined in'\
         'Equation~\ref{eq:double_CB} and~\ref{eq:single_CB}. All parameters'\
         'except $\mu$ and $\sigma$ are fixed in the fit to data.}' \
                                          >> $TAB_FILE
echo '\label{tab:MC_signal_params}'       >> $TAB_FILE
echo '\end{table}'                        >> $TAB_FILE
echo '\end{document}'                     >> $TAB_FILE
    

