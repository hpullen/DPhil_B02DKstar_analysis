# Make tables with raw yields
#!/usr/bin/env bash 
source ../s_no.sh

# Get yield for a component
get_yield() {
    MODE=$1 
    RUN=$2 
    FLAV=$3
    COMP=$4
    INFILE="../../../Fit_data/Results/yields_split.param"
    PAR="N_${COMP}_${MODE}_${RUN}_${FLAV}"
    n_no $(awk "/^${PAR} /{print \$2, \$3}" $INFILE)
}

# Make single table
make_table() {

    # Parameters
    MODE=$1
    DESC=$2
    shift && shift 
    RUNS=$@

    # Settings
    OUTFILE="../../../ANA_resources/Tables/Data_fit/yields_${MODE}.tex"
    NAMEFILE="../../mode_names.param"
    COMPFILE="../../comp_names.param"

    # Table format
    FORMAT="c"
    for RUN in $RUNS; do
        FORMAT="${FORMAT}cc"
    done

    # Start table
    echo '\begin{table}' > $OUTFILE
    echo '  \centering' >> $OUTFILE
    echo "  \\begin{tabular}{${FORMAT}}" >> $OUTFILE
    echo '      \toprule' >> $OUTFILE >> $OUTFILE

    # Make header
    HEADER=""
    SUBHEADER=""
    for RUN in $RUNS; do
        if [[ $RUN == "run1" ]]; then
            RUN_NAME='Run 1'
        else 
            RUN_NAME='Run 2'
        fi
        HEADER="${HEADER} & \\multicolumn{2}{c}{$RUN_NAME}"
        SUBHEADER="${SUBHEADER}& \$\\bar{B}^0\$ & \$B^0\$"
    done
    HEADER="${HEADER} \\\\"
    SUBHEADER="${SUBHEADER} \\\\"
    echo "      $HEADER" >> $OUTFILE
    echo "      $SUBHEADER" >> $OUTFILE
    echo '      \midrule' >> $OUTFILE

    # Fill yields
    COMPS="signal expo Bs low Bs_low DKpipi rho"
    for COMP in $COMPS; do
        LINE=$(grep "^$COMP " $COMPFILE | sed "s/^$COMP //")
        for RUN in $RUNS; do
            LINE="${LINE} & $(get_yield $MODE $RUN "minus" $COMP)"
            LINE="${LINE} & $(get_yield $MODE $RUN "plus" $COMP)"
        done
        LINE="${LINE} \\\\"
        echo "      $LINE" >> $OUTFILE
    done

    # End table
    echo '      \bottomrule' >> $OUTFILE
    echo '      \end{tabular}' >> $OUTFILE
    CAPTION="  \\caption{Raw yields for each signal and background component in the \$${DESC}\$ mode"
    if [[ $MODE == "pipipipi" ]]; then
        CAPTION="${CAPTION}.}"
    else 
        CAPTION="${CAPTION} in each run.}" >> $OUTFILE
    fi
    echo $CAPTION >> $OUTFILE
    echo "\\label{tab:yields_${MODE}}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE
}


# Make all tables
make_table Kpi 'K\pi' run1 run2
make_table piK '\pi K' run1 run2
make_table KK 'KK' run1 run2
make_table pipi '\pi\pi' run1 run2
make_table Kpipipi 'K\pi\pi\pi' run1 run2
make_table piKpipi '\pi K\pi\pi' run1 run2
make_table pipipipi '\pi\pi\pi\pi' run2
