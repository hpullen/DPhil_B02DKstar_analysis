# Make table of systematic uncertainties
#!/usr/bin/env bash 
source ../s_no.sh

# Make one line
make_line() {

    # Setup
    TYPE=$1
    CAP=$2
    SYS_FILE="../../../Systematics/Results/${TYPE}.param"

    # Find uncertainty for each observable
    LINE="$CAP"
    for PAR in $PARS; do
        if grep "${PAR}[\ _]" $SYS_FILE > /dev/null; then
            SYS=$(printf '%.4f' $(grep "${PAR}[\ _]" $SYS_FILE | awk '{print $2}'))
            LINE="${LINE} & $SYS"
        else 
            LINE="${LINE} & \\--"
        fi
    done
    echo "      $LINE \\\\" >> $OUTFILE

}

# Make one table
make_one() {

    # Parameters
    NAME=$1
    DESC=$2
    shift && shift
    PARS=$@
    N_PARS=$#

    # Settings
    OUTDIR="../../../ANA_resources/Tables/Systematics/"
    OUTFILE="${OUTDIR}/${NAME}_systematics.tex"

    # Table formatting
    FORMAT="c"
    for PAR in $PARS; do
        FORMAT="${FORMAT}c"
    done

    # Start table
    echo '\begin{table}' > $OUTFILE
    echo '  \centering' >> $OUTFILE
    echo "  \\begin{tabular}{$FORMAT}" >> $OUTFILE
    echo '      \toprule' >> $OUTFILE
    echo "      Source & \\multicolumn{$N_PARS}{c}{Observable} \\\\" >> $OUTFILE
    echo '      \midrule' >> $OUTFILE

    # Fill statistical
    LINE="Statistical"
    STAT_FILE="raw_obs.param"
    for PAR in $PARS; do
        STAT=$(n_no $(grep $PAR $STAT_FILE | awk '{print $2, $3}') | awk '{print $3}' | sed 's/\$$//')
        LINE="${LINE} & $STAT"
    done
    LINE="${LINE} \\\\"
    echo "      $LINE" >> $OUTFILE
    echo '      \midrule' >> $OUTFILE

    # Fill each item
    make_line branching_ratios 'Branching ratios'
    make_line selection_efficiency 'Selection efficiency'
    make_line production_asymmetry 'Production asymmetry'
    make_line detection_asymmetry 'Detection asymmetry'
    make_line fs_fd '$f_s/f_d$'

    # Total squared systematic

    # Finish table
    echo '      \bottomrule' >> $OUTFILE
    echo '  \end{tabular}' >> $OUTFILE
    echo "  \\caption{Systematic uncertaintyies plots for ${DESC} parameters of interest. Where the systematic uncetainty is more than two orders of magnitude smaller than the statistical, no value is given. The total is calculated by adding all sources in quadrature.}" >> $OUTFILE 
    echo "\\label{tab:${NAME}_systematics}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE
}


# root -b -q print_obs.C > /dev/null
make_one twoBody_ADS 'two-body ADS' A_signal_Kpi R_signal_piK_plus R_signal_piK_minus A_Bs_piK
make_one KK_run1 'Run 1 GLW' A_signal_KK_run1 R_signal_KK_run1 A_Bs_KK_run1 R_ds_KK_run1 A_signal_pipi_run1 R_signal_pipi_run1 A_Bs_pipi_run1 R_ds_pipi_run1
make_one KK_run2 'Run 2 GLW' A_signal_KK_run2 R_signal_KK_run2 A_Bs_KK_run2 R_ds_KK_run2 A_signal_pipi_run2 R_signal_pipi_run2 A_Bs_pipi_run2 R_ds_pipi_run2
make_one fourBody '4-body' A_signal_Kpipipi R_signal_piKpipi_plus R_signal_piKpipi_minus A_Bs_piKpipi A_signal_pipipipi_run2 R_signal_pipipipi_run2 A_Bs_pipipipi_run2 R_ds_pipipipi_run2
