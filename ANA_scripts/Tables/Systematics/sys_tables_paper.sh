# Make table of systematic uncertainties
#!/usr/bin/env bash 
source ../s_no.sh

# Make one line
make_line() {

    # Setup
    TYPE=$1
    CAP=$2
    DIR=""
    SYS_FILE="../../../Systematics/Results/${DIR}${TYPE}.param"

    # Find uncertainty for each observable
    LINE="$CAP"
    for PAR in $PARS; do
        if grep "${PAR}[\ ]" $SYS_FILE > /dev/null; then
            SYS=$(grep "${PAR}[\ ]" $SYS_FILE | awk '{print $2}')
            if [[ $SYS == "0" ]]; then
                LINE="${LINE} & -"
            else 
                if grep "signal_piK" <<< $PAR >/dev/null; then
                    SYS_SHORT=$(printf '%.3f' $SYS)
                    if [[ $(bc -l <<< "$SYS_SHORT < 0.001") == 1 ]]; then
                        SYS_SHORT="$<0.001$"
                    fi
                else 
                    SYS_SHORT=$(printf '%.2f' $SYS)
                    if [[ $(bc -l <<< "$SYS_SHORT < 0.01") == 1 ]]; then
                        SYS_SHORT="$<0.01$"
                    fi
                fi
                LINE="${LINE} & $SYS_SHORT"
            fi
        else 
            LINE="${LINE} & - "
        fi
    done
    echo "      $LINE \\\\" >> $OUTFILE

}

# Make one table
make_one() {

    # Parameters
    NAME=$1
    shift 
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
    echo "  \\caption{Systematic uncertainties for the observables. Systematic uncertainties more than two orders of magnitude smaller than the statistical uncertainty are neglected. The total systematic uncertainty is calculated by summing all sources in quadrature.}" >> $OUTFILE 
    echo "  \\begin{tabular}{$FORMAT}" >> $OUTFILE
    echo '      \toprule' >> $OUTFILE
    echo "      Source & \\multicolumn{$N_PARS}{c}{Observable} \\\\" >> $OUTFILE
    echo '      \midrule' >> $OUTFILE

    # Fill names
    LINE=""
    NAMES_FILE=../../names_paper.param
    for PAR in $PARS; do
        if grep "${PAR}[\ ]" $NAMES_FILE >/dev/null; then
            LATEX_NAME=$(grep "${PAR}[\ ]" $NAMES_FILE | sed 's/^[^\ ]*\ //')
            LINE="${LINE} & $LATEX_NAME"
        else
            echo $PAR not found in $NAMES_FILE !
            exit -1
        fi
    done
    echo "      $LINE \\\\" >> $OUTFILE
    echo '      \midrule' >> $OUTFILE

    # Fill statistical
    LINE="Statistical"
    STAT_FILE="../../../Systematics/final_result.param"
    for PAR in $PARS; do
        STAT=$(n_no $(grep $PAR $STAT_FILE | awk '{print $2, $3}') | awk '{print $3}' | sed 's/\$$//')
        LINE="${LINE} & $STAT"
    done
    LINE="${LINE} \\\\"
    echo "      $LINE" >> $OUTFILE
    echo '      \midrule' >> $OUTFILE

    # Fill each item
    make_line selection_efficiency 'Selection efficiency'
    make_line PID 'PID efficiency'
    make_line branching_ratios 'Branching ratios'
    make_line production_asymmetry 'Production asymmetry'
    make_line detection_asymmetry 'Detection asymmetry'
    # make_line fs_fd '$f_s/f_d$'
    make_line signal_shape_pars 'Signal shape parameters'
    make_line Bs_low_shape_pars '$B^0_s \to D^* K^{*0}$ shape parameters'
    make_line background_shape_pars 'Other background shape parameters'
    make_line gamma_pi_inputs '$D^* \to D^0 \gamma/\pi^0$ inputs'
    make_line rho_PID '$B\to D\pi\pi$ PID correction'
    make_line DKpipi_inputs '$B\to DK\pi\pi$ inputs'
    # make_line pipipipi_low '$F_CP$ and $r_B$ inputs'
    make_line charmless 'Charmless background'
    # make_line bias_correction 'Fitter bias correction'

    # Total squared systematic
    echo '      \midrule' >> $OUTFILE
    make_line sum 'Total systematic'

    # Finish table
    echo '      \bottomrule' >> $OUTFILE
    echo '  \end{tabular}' >> $OUTFILE
    echo "\\label{tab:systematics}" >> $OUTFILE
    echo '\end{sidewaystable}' >> $OUTFILE
}


# Normal result
make_one paper A_signal_KK A_signal_pipi R_signal_KK R_signal_pipi A_signal_pipipipi R_signal_pipipipi R_signal_piK_plus R_signal_piK_minus R_signal_piKpipi_plus R_signal_piKpipi_minus A_signal_Kpi A_signal_Kpipipi
