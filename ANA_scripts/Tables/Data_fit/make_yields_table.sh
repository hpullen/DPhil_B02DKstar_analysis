# Make tables with raw yields
#!/usr/bin/env bash 
source ../s_no.sh

# Get yield for a component
get_yield() {
    MODE=$1 
    RUN=$2
    COMP=$3
    INFILE="../../../Fit_data/Results/yields_combined.param"
    if [[ $RUN == "both" ]]; then
        INFILE="../../../Fit_data/Results/yields_combinedRuns_combined.param"
    fi
    PAR="N_${COMP}_${MODE}"
    if [[ $RUN != "both" ]]; then
        PAR="${PAR}_${RUN}"
    fi
    if grep "^$PAR " $INFILE > "/dev/null"; then
        n_no $(awk "/^${PAR} /{print \$2, \$3}" $INFILE)
    else
        if [[ $COMP == "signal" ]]; then
            echo "??"
        else 
            echo '\--'
        fi
    fi
}

# Make single table
make_table() {

    # Parameters
    NAME=$1
    RUN=$2
    DESC=$3
    shift && shift && shift
    MODES=$@

    # Settings
    OUTFILE="../../../ANA_resources/Tables/Data_fit/yields_${NAME}.tex"
    NAMEFILE="../../mode_names.param"
    COMPFILE="../../comp_names.param"

    # Table format
    FORMAT="c"
    for MODE in $MODES; do
        FORMAT="${FORMAT}c"
    done

    # Start table
    echo '\begin{table}' > $OUTFILE
    echo '  \centering' >> $OUTFILE
    echo "  \\begin{tabular}{${FORMAT}}" >> $OUTFILE
    echo '      \toprule' >> $OUTFILE >> $OUTFILE

    # Make header
    HEADER=""
    for MODE in $MODES; do
        HEADER="${HEADER} & $(grep "^$MODE " $NAMEFILE | sed "s/^$MODE //")"
    done
    HEADER="${HEADER} \\\\"
    echo "      $HEADER" >> $OUTFILE
    echo '      \midrule' >> $OUTFILE

    # Fill yields
    COMPS="signal expo Bs low Bs_low DKpipi rho"
    for COMP in $COMPS; do
        LINE=$(grep "^$COMP " $COMPFILE | sed "s/^$COMP //")
        for MODE in $MODES; do
            LINE="${LINE} & $(get_yield $MODE $RUN $COMP)"
        done
        LINE="${LINE} \\\\"
        echo "      $LINE" >> $OUTFILE
    done

    # End table
    echo '      \bottomrule' >> $OUTFILE
    echo '      \end{tabular}' >> $OUTFILE
    echo "  \\caption{Raw yields for each signal and background component obtained from a fit to $DESC data summed over \$B^0\$ and \$\\bar{B}^0\$.}" >> $OUTFILE
    echo "\\label{tab:yields_combined_${NAME}}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE
}


# Make all tables
make_table 2body_run1 run1 'two-body Run 1' Kpi piK KK pipi
make_table 2body_run2 run2 'two-body Run 2' Kpi piK KK pipi
make_table 4body_run1 run1 'four-body Run 1' Kpipipi piKpipi
make_table 4body_run2 run2 'four-body Run 2' Kpipipi piKpipi pipipipi
make_table 2body both 'two-body' Kpi piK KK pipi
make_table 4body both 'four-body' Kpipipi piKpipi
