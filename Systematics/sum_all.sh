ALL_PARS="R_signal_piK_minus_blind R_signal_piK_plus_blind R_signal_piKpipi_minus_blind R_signal_piKpipi_plus_blind A_signal_Kpi A_signal_Kpipipi A_Bs_piK A_Bs_piKpipi R_signal_KK_run1_blind A_signal_KK_run1_blind R_ds_KK_run1_blind A_Bs_KK_run1 R_signal_pipi_run1_blind A_signal_pipi_run1_blind R_ds_pipi_run1_blind A_Bs_pipi_run1 R_signal_pipi_run2_blind A_signal_pipi_run2_blind R_ds_pipi_run2_blind A_Bs_pipi_run2 R_signal_KK_run2_blind A_signal_KK_run2_blind R_ds_KK_run2_blind A_Bs_KK_run2 R_signal_pipipipi_run2_blind A_signal_pipipipi_run2_blind R_ds_pipipipi_run2_blind A_Bs_pipipipi_run2"

SOURCES="branching_ratios detection_asymmetry DKpipi_inputs fs_fd four_vs_two PID selection_efficiency production_asymmetry gamma_pi_branching_ratios gamma_pi_selection signal_shape_pars"

OUTFILE=Results/sum.param
if [[ -f $OUTFILE ]]; then rm $OUTFILE; fi
for PAR in $ALL_PARS; do
    SUM="0.0"
    for SYS in $SOURCES; do
        INFILE="Results/${SYS}.param"
        echo $SYS $PAR
        if grep "^$PAR\ " $INFILE > /dev/null; then
            ERR=$(awk "/^$PAR\ /{print \$2}" $INFILE)
            SUM=$(bc -l <<< "$SUM + $ERR * $ERR")
        fi
    done 
    echo "$PAR $(bc -l <<< "sqrt($SUM)")" >> $OUTFILE
done
