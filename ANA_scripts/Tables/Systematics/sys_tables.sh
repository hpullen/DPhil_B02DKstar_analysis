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
        if grep "${PAR}[\ ]" $SYS_FILE > /dev/null; then
            SYS=$(grep "${PAR}[\ ]" $SYS_FILE | awk '{print $2}')
            if [[ $SYS == "0" ]]; then
                LINE="${LINE} & 0.0"
            else 
                SYS_SHORT=$(n_no 0.1 $SYS | awk '{print $3}' | sed 's/\$$//')
                LINE="${LINE} & $SYS_SHORT"
            fi
        else 
            LINE="${LINE} & 0.0 "
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
    echo '\begin{sidewaystable}' > $OUTFILE
    echo '  \centering' >> $OUTFILE
    echo "  \\begin{tabular}{$FORMAT}" >> $OUTFILE
    echo '      \toprule' >> $OUTFILE
    echo "      Source & \\multicolumn{$N_PARS}{c}{Observable} \\\\" >> $OUTFILE
    echo '      \midrule' >> $OUTFILE

    # Fill names
    LINE=""
    for PAR in $PARS; do
        LATEX_NAME=$(grep "${PAR}[\ ]" ../../names.param | sed 's/^[^\ ]*\ //')
        LINE="${LINE} & $LATEX_NAME"
    done
    echo "      $LINE \\\\" >> $OUTFILE
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
    make_line PID 'PID efficiency'
    make_line production_asymmetry 'Production asymmetry'
    make_line detection_asymmetry 'Detection asymmetry'
    make_line fs_fd '$f_s/f_d$'
    make_line signal_shape_pars 'Signal shape parameters'
    make_line Bs_low_shape_pars '$B^0_s \to D^* K^{*0}$ shape parameters'
    make_line background_shape_pars 'Other background shape parameters'
    make_line gamma_pi_inputs '$D^* \to D^0 \gamma/\pi^0$ inputs'
    # make_line rho_PID '$B\to D\pi\pi PID correction'
    make_line DKpipi_inputs '$B\to DK\pi\pi$ inputs'
    # make_line pipipipi_low '$F_CP$ and $r_B$ inputs'
    make_line charmless 'Charmless background'
    make_line bias_correction 'Fitter bias correction'

    # Total squared systematic
    echo '      \midrule' >> $OUTFILE
    make_line sum 'Total systematic'

    # Finish table
    echo '      \bottomrule' >> $OUTFILE
    echo '  \end{tabular}' >> $OUTFILE
    echo "  \\caption{Systematic uncertainties for ${DESC} parameters of interest. Where the systematic uncetainty is more than two orders of magnitude smaller than the statistical, a value of zero is given. The total is calculated by adding all sources in quadrature.}" >> $OUTFILE 
    echo "\\label{tab:${NAME}_systematics}" >> $OUTFILE
    echo '\end{sidewaystable}' >> $OUTFILE
}


# root -b -q print_obs.C > /dev/null
cd ../../../Systematics/ && ./FinalResult && cd -
make_one twoBody_ADS 'two-body ADS' A_signal_Kpi R_signal_piK_plus_blind R_signal_piK_minus_blind A_Bs_piK
make_one KK_run1 'Run 1 GLW' A_signal_KK_run1_blind R_signal_KK_run1_blind A_Bs_KK_run1 R_ds_KK_run1_blind A_signal_pipi_run1_blind R_signal_pipi_run1_blind A_Bs_pipi_run1 R_ds_pipi_run1_blind
make_one KK_run2 'Run 2 GLW' A_signal_KK_run2_blind R_signal_KK_run2_blind A_Bs_KK_run2 R_ds_KK_run2_blind A_signal_pipi_run2_blind R_signal_pipi_run2_blind A_Bs_pipi_run2 R_ds_pipi_run2_blind
make_one fourBody '4-body' A_signal_Kpipipi R_signal_piKpipi_plus_blind R_signal_piKpipi_minus_blind A_Bs_piKpipi A_signal_pipipipi_run2_blind R_signal_pipipipi_run2_blind A_Bs_pipipipi_run2 R_ds_pipipipi_run2_blind
