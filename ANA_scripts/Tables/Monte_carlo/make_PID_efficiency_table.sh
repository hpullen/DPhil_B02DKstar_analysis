####################################################################################
# Makes a tex file containing a table of PID efficiencies for each year and polarity
####################################################################################
source ../s_no.sh

# Print a line
print_line() {

    # Mode and year
    MODE=$1
    YEAR=$2

    # Start table line
    LINE="$YEAR"

    # Loop through polarities and flavours
    for PARENT in "B0" "B0bar"; do
        INFILE=/home/pullen/analysis/B02DKstar/Efficiencies/PID/Results/${MODE}_${PARENT}.param
        for MAG in "down" "up"; do

            # Find value
            if grep "${YEAR}_${MAG}" $INFILE > /dev/null; then 
                VALUE=$(awk "/${YEAR}_${MAG}/{print \$2 * 100}" $INFILE)
                ERROR=$(awk "/${YEAR}_${MAG}/{print \$3 * 100}" $INFILE)
                LINE="${LINE} & $(n_no $VALUE $ERROR)"
            else 
                LINE="${LINE} & - "
            fi

        done
    done

    # Return line
    LINE=$LINE' \\'
    echo $LINE
}


# Print a run-averaged line
print_run_line() {

    # Mode and run
    MODE=$1
    RUN=$2

    # Start line
    LINE="Run $RUN combined"

    # Loop through parent types
    for PARENT in "B0" "B0bar"; do
        INFILE=/home/pullen/analysis/B02DKstar/Efficiencies/Values/PID_efficiency_${PARENT}_run${RUN}.param
        VALUE=$(awk "/^${MODE} /{print \$2 * 100}" $INFILE)
        ERROR=$(awk "/^${MODE} /{print \$3 * 100}" $INFILE)
        LINE="$LINE & \\multicolumn{2}{c}{$(n_no $VALUE $ERROR)}"
    done

    # Return line
    LINE=$LINE' \\'
    echo $LINE

}


# Make list of years for caption
make_year_list() {

    # Starting settings
    MODE=$1
    COUNT=1
    TOTAL=0
    LIST=""

    # Count number of years used
    for YEAR in 2011 2012 2015 2016; do
        PIDFILE=/home/pullen/analysis/B02DKstar/Efficiencies/PID/Results/${MODE}_B0.param
        if grep "$YEAR" $PIDFILE > /dev/null; then
            TOTAL=$((TOTAL + 1))
        fi
    done

    # Add years to list
    for YEAR in 2011 2012 2015 2016; do
        PIDFILE=/home/pullen/analysis/B02DKstar/Efficiencies/PID/Results/${MODE}_B0.param
        if grep "$YEAR" $PIDFILE > /dev/null; then
            if [[ $COUNT -eq 1 ]]; then
                LIST=$YEAR
            elif [[ $COUNT -eq $TOTAL ]]; then
                LIST="$LIST and $YEAR"
            else 
                LIST="$LIST, $YEAR"
            fi
            COUNT=$((COUNT + 1))
        fi
    done

    # Return list
    echo $LIST
}


# Make a single table
make_single_table() {

    # Location of analysis directory and table to create
    MODE=$1
    DECAY_STR=$2
    ANALYSIS_DIR="/home/pullen/analysis/B02DKstar/"
    TAB_FILE="${ANALYSIS_DIR}/ANA_resources/Tables/Monte_carlo/PID/PID_efficiency_table_${MODE}.tex"

    # Start table
    echo '\begin{table}[H]'                          > $TAB_FILE
    echo '    \centering'                            >> $TAB_FILE
    echo '    \begin{tabular}{ccccc}'                  >> $TAB_FILE
    echo '        \toprule'                          >> $TAB_FILE
    echo '        & \multicolumn{2}{c}{$B^0$} & '\
                  '\multicolumn{2}{c}{$\bar{B}^0$}\\' >> $TAB_FILE
    echo '        & MagDown & MagUp & MagDown & MagUp\\' >> $TAB_FILE
    echo '        \midrule'                          >> $TAB_FILE

    # Fill in entries by year
    if [[ $MODE != "pipipipi" ]]; then
        echo $(print_line $MODE 2011) >> $TAB_FILE
        echo $(print_line $MODE 2012) >> $TAB_FILE
    fi
    echo $(print_line $MODE 2015) >> $TAB_FILE
    echo $(print_line $MODE 2016) >> $TAB_FILE
    echo '        \midrule'                          >> $TAB_FILE

    # Fill in run averages
    if [[ $MODE != "pipipipi" ]]; then
        echo $(print_run_line $MODE 1) >> $TAB_FILE
    fi
    echo $(print_run_line $MODE 2) >> $TAB_FILE

    # End table
    echo '        \bottomrule'                       >> $TAB_FILE
    echo '    \end{tabular}'                         >> $TAB_FILE
    echo "    \\caption{PID efficiencies for ${DECAY_STR} for $(make_year_list $MODE) MC for both"\
         "magnet polarities and parent flavours, given as percentages. The quoted "\
         "uncertainties come from the finite reference sample size.}" >> $TAB_FILE
    echo "\\label{tab:PID_efficiency_${MODE}}"       >> $TAB_FILE
    echo '\end{table}'                               >> $TAB_FILE

}

# Process all
make_single_table Kpi '$D \to K\pi$'
make_single_table KK '$D \to KK$'
make_single_table pipi '$D \to \pi\pi$'
make_single_table Kpipipi '$D \to K\pi\pi\pi$'
make_single_table pipipipi '$D \to \pi\pi\pi\pi$'
