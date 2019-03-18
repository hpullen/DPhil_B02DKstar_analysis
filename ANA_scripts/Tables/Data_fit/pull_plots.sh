# Copy pull plots to ANA_resources and make figures
#!/usr/bin/env bash 

# Make one figure
make_one() {

    # Parameters
    NAME=$1
    DESC=$2
    TYPE=$3
    EXTRA=""
    if [[ $TYPE == split ]]; then
        EXTRA="/split/"
    elif [[ $TYPE == "splitObs" ]]; then
        EXTRA="/SplitObs/"
    fi
    shift && shift && shift
    PARS=$@

    # Settings
    OUTDIR="../../../ANA_resources/Plots/Data_fit/FitterBias/$EXTRA"
    SHORTDIR="ANA_resources/Plots/Data_fit/FitterBias/$EXTRA"
    OUTFILE="${OUTDIR}/${NAME}_pulls.tex"
    INDIR="../../../Biases/Plots/$EXTRA"

    # Copy figures
    for PAR in $PARS; do
        cp -f "${INDIR}/${PAR}.pdf" $OUTDIR
    done

    # Start figure
    echo '\begin{figure}' > $OUTFILE
    echo '  \centering' >> $OUTFILE
    echo '  \begin{tabular}{c}' >> $OUTFILE

    # Add graphics
    for PAR in $PARS; do
        echo "\\includegraphics[width=\\textwidth]{${SHORTDIR}/${PAR}.pdf} \\\\" >> $OUTFILE
    done

    # Finish figure
    echo '  \end{tabular}' >> $OUTFILE
    echo "  \\caption{Pull plots for ${DESC} parameters of interest, obtained from generating and fitting 3000 toys using the data fit model. The left hand plot shows the fitted parameter distribution, with the value used to generate the parameter indicated with a dotted red line. The central plot shows the statistical uncertainty on the parameter obtained from the fit, with the statistical uncertainty of the parameter in the real fit to data indicated with a dotted red line. The right hand plot shows the pull distribution fitted with a Gaussian.}" >> $OUTFILE
    echo "\\label{fig:${NAME}${EXTRA}_pulls}" >> $OUTFILE
    echo '\end{figure}' >> $OUTFILE
}


# Split runs version
make_one twoBody_ADS 'two-body ADS' split A_signal_Kpi R_signal_piK_plus R_signal_piK_minus A_Bs_piK
make_one fourBody_ADS 'four-body ADS' split A_signal_Kpipipi R_signal_piKpipi_plus R_signal_piKpipi_minus A_Bs_piKpipi
make_one KK '$KK$' split A_signal_KK R_signal_KK A_Bs_KK R_Bs_KK_run1 R_Bs_KK_run2
make_one pipi '$\pi\pi$' split A_signal_pipi R_signal_pipi A_Bs_pipi R_Bs_pipi_run1 R_Bs_pipi_run2
make_one 4pi '$4\pi$' split A_signal_pipipipi R_signal_pipipipi A_Bs_pipipipi R_Bs_pipipipi_run2



# make_one twoBody_ADS 'two-body ADS' false A_signal_Kpi R_signal_piK_plus R_signal_piK_minus A_Bs_piK
# make_one fourBody_ADS 'four-body ADS' false A_signal_Kpipipi R_signal_piKpipi_plus R_signal_piKpipi_minus A_Bs_piKpipi
# make_one KK_run1 'Run 1 $KK$' false A_signal_KK_run1 R_signal_KK_run1 A_Bs_KK_run1 R_ds_KK_run1
# make_one KK_run2 'Run 2 $KK$' false A_signal_KK_run2 R_signal_KK_run2 A_Bs_KK_run2 R_ds_KK_run2
# make_one pipi_run1 'Run 1 $\pi\pi$' false A_signal_pipi_run1 R_signal_pipi_run1 A_Bs_pipi_run1 R_ds_pipi_run1
# make_one pipi_run2 'Run 2 $\pi\pi$' false A_signal_pipi_run2 R_signal_pipi_run2 A_Bs_pipi_run2 R_ds_pipi_run2
# make_one 4pi '4$\pi$' false A_signal_pipipipi_run2 R_signal_pipipipi_run2 A_Bs_pipipipi_run2 R_ds_pipipipi_run2

# # Combined runs version
# make_one twoBody_ADS 'two-body ADS' true A_signal_Kpi R_signal_piK_plus R_signal_piK_minus A_Bs_piK
# make_one fourBody_ADS 'four-body ADS' true A_signal_Kpipipi R_signal_piKpipi_plus R_signal_piKpipi_minus A_Bs_piKpipi
# make_one KK '$KK$' true A_signal_KK R_signal_KK A_Bs_KK R_ds_KK
# make_one pipi '$KK$' true A_signal_pipi R_signal_pipi A_Bs_pipi R_ds_pipi
