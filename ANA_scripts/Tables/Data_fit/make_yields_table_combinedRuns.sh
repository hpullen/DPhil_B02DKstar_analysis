# Make tables with raw yields
#!/usr/bin/env bash 
source ../s_no.sh

# Get yield for a component
get_yield() {
    MODE=$1 
    FLAV=$2
    COMP=$3
    INFILE="../../../Fit_data/Results/yields_split.param"
    PAR1="N_${COMP}_${MODE}_run1_${FLAV}"
    PAR2="N_${COMP}_${MODE}_run2_${FLAV}"
    if [[ $MODE == "pipipipi" ]]; then
        n_no $(awk "/^${PAR2} /{print \$2, \$3}" $INFILE)
    else 
        VAL1=$(awk "/^${PAR1} /{print \$2}" $INFILE)
        VAL2=$(awk "/^${PAR2} /{print \$2}" $INFILE)
        VAL=$(bc -l <<< "$VAL1 + $VAL2")
        ERR1=$(awk "/^${PAR1} /{print \$3}" $INFILE)
        ERR2=$(awk "/^${PAR2} /{print \$3}" $INFILE)
        ERR=$(bc -l <<< "sqrt($ERR1^2 + $ERR2^2)")
        n_no $VAL $ERR
    fi
}

# Make single table
make_table() {

    # Parameters
    NAME=$1
    DESC=$2
    shift && shift 
    MODES=$@

    # Settings
    OUTFILE="../../../ANA_resources/Tables/Data_fit/yields_${NAME}.tex"
    NAMEFILE="../../mode_names.param"
    COMPFILE="../../comp_names.param"

    # Table format
    FORMAT="c"
    for MODE in $MODES; do
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
    for MODE in $MODES; do
        HEADER="${HEADER} & \\multicolumn{2}{c}{$(grep "^$MODE " $NAMEFILE | sed "s/^$MODE //")}"
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
        for MODE in $MODES; do
            LINE="${LINE} & $(get_yield $MODE "minus" $COMP)"
            LINE="${LINE} & $(get_yield $MODE "plus" $COMP)"
        done
        LINE="${LINE} \\\\"
        echo "      $LINE" >> $OUTFILE
    done

    # End table
    echo '      \bottomrule' >> $OUTFILE
    echo '      \end{tabular}' >> $OUTFILE
    CAPTION="  \\caption{Raw yields for each signal and background component obtained from a fit to $DESC data"
    if [[ $NAME == "4pi" ]]; then
        CAPTION="${CAPTION}.}"
    else 
        CAPTION="${CAPTION}, summed over Run 1 and Run 2.}" >> $OUTFILE
    fi
    echo $CAPTION >> $OUTFILE
    echo "\\label{tab:yields_split_${NAME}}" >> $OUTFILE
    echo '\end{table}' >> $OUTFILE
}


# Make all tables
make_table 2body_ADS 'two-body ADS' Kpi piK 
make_table 2body_GLW 'two-body GLW' KK pipi
make_table 4body_ADS 'four-body ADS' Kpipipi piKpipi 
make_table 4pi 'the $4\pi$' pipipipi
