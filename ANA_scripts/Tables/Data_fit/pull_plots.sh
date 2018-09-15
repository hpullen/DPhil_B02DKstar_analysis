# Copy pull plots to ANA_resources and make figures
#!/usr/bin/env bash 

# Make one figure
make_one() {

    # Parameters
    NAME=$1
    DESC=$2
    shift && shift
    PARS=$@

    # Settings
    OUTDIR="../../../ANA_resources/Plots/Data_fit/FitterBias/"
    SHORTDIR="ANA_resources/Plots/Data_fit/FitterBias/"
    OUTFILE="${OUTDIR}/${NAME}_pulls.tex"
    INDIR="../../../Fit_data_new/Plots/FitterBias/"

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
        echo "\\includegraphics[width=0.7\\textwidth]{${SHORTDIR}/${PAR}.pdf} \\\\" >> $OUTFILE
    done

    # Finish figure
    echo '  \end{tabular}' >> $OUTFILE
    echo "  \\caption{Pull plots for ${DESC} parameters of interest, obtained from generating and fitting 1000 toys using the data fit model.}" >> $OUTFILE
    echo "\\label{fig:${NAME}_pulls}" >> $OUTFILE
    echo '\end{figure}' >> $OUTFILE
}


make_one twoBody_ADS 'two-body ADS' A_signal_Kpi R_signal_piK_plus R_signal_piK_minus A_Bs_piK
make_one fourBody_ADS 'four-body ADS' A_signal_Kpipipi R_signal_piKpipi_plus R_signal_piKpipi_minus A_Bs_piKpipi
make_one GLW_run1 'Run 1 GLW' A_signal_KK_run1 R_signal_KK_run1 A_Bs_KK_run1 R_ds_KK_run1 A_signal_pipi_run1 R_signal_pipi_run1 A_Bs_pipi_run1 R_ds_pipi_run1
make_one GLW_run2 'Run 2 GLW' A_signal_KK_run2 R_signal_KK_run2 A_Bs_KK_run2 R_ds_KK_run2 A_signal_pipi_run2 R_signal_pipi_run2 A_Bs_pipi_run2 R_ds_pipi_run2
make_one 4pi '4$\pi$' A_signal_pipipipi_run2 R_signal_pipipipi_run2 A_Bs_pipipipi_run2 R_ds_pipipipi_run2