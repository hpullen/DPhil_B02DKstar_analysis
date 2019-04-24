#!/usr/bin/env bash 

# Get power of 10 of number
get_exponent() {

    # Value and log
    VALUE=$1

    # Choose what to round
    if [[ $(bc -l <<< "$VALUE > 1") == 1 ]]; then
        POS_LOG=TRUE
        TO_ROUND=$(bc -l <<< "l($VALUE)/l(10)")
    else 
        POS_LOG=FALSE
        TO_ROUND=$(bc -l <<< "l(1/$VALUE)/l(10) + 0.9999999999")
    fi

    # Round down
    if [[ $(bc -l <<< "($TO_ROUND < 1) * ($TO_ROUND >= 0)") == 1 ]] ; then
        ROUNDED=0
    elif [[ $(bc -l <<< "($TO_ROUND < 0) * ($TO_ROUND >= -1)") == 1 ]]; then
        ROUNDED=-1
    else 
        ROUNDED=$(echo $TO_ROUND | sed 's/^\([^\.]\+\).*/\1/')
    fi

    # Return
    if [[ $POS_LOG == "TRUE" ]]; then
        echo $ROUNDED
    else 
        echo "-"${ROUNDED}
    fi

}


# Main function
s_no() {

    # Get value and error
    VALUE=$1
    ERROR=$2
    SYS=""
    if [[ $# == 3 ]]; then
        SYS=$3
    fi

    # Get sign of value
    if [[ $(bc -l <<< "$VALUE < 0") == 1 ]]; then
        SIGN="-"
        VALUE=$(echo $VALUE | sed "s/-//")
    else
        SIGN=""
    fi

    # Check for zero value
    if [[ $(bc -l <<< "$VALUE == 0") == 1 ]]; then

        EXP=$(get_exponent $ERROR)
        N_DP=0

    else 

        # Work out number of decimal places
        N_DP_VALUE=$(get_exponent $VALUE)
        N_DP_ERROR=$(get_exponent $ERROR)
        if [[ $(bc -l <<< "$N_DP_ERROR > $N_DP_VALUE") == 1 ]]; then
            EXP_VAL=$ERROR
            N_DP=0
        else
            EXP_VAL=$VALUE
            N_DP=$((N_DP_VALUE - N_DP_ERROR))
        fi

        # Find exponent
        EXP=$(get_exponent $EXP_VAL)

    fi

    # Get value and error corrected for exponent
    VALUE_CORR=$(bc -l <<< "$VALUE / 10 ^ $EXP")
    ERROR_CORR=$(bc -l <<< "$ERROR / 10 ^ $EXP")
    if [[ $SYS != "" ]]; then
        SYS_CORR=$(bc -l <<< "$SYS / 10 ^ $EXP")
    fi

    # Correct DP if first digit is < 355
    FIRST_ERROR_DIGIT=$(echo $ERROR | sed 's/[\.0]*\([^\.0]\{3\}\).*/\1/')
    if [[ $FIRST_ERROR_DIGIT < 355 ]]; then
        N_DP=$((N_DP + 1))
    fi

    # Get truncated values
    VALUE_FINAL=$(printf "%.${N_DP}f" $VALUE_CORR)
    ERROR_FINAL=$(printf "%.${N_DP}f" $ERROR_CORR)
    if [[ $SYS != "" ]]; then
        SYS_FINAL=$(printf "%.${N_DP}f" $SYS_CORR)
    fi

    # Print in scientific notation
    if [[ $SYS == "" ]]; then
        if [[ $EXP == 0 ]]; then
            echo "\$${SIGN}${VALUE_FINAL} \\pm ${ERROR_FINAL}\$"
        else
            echo "\$(${SIGN}${VALUE_FINAL} \\pm ${ERROR_FINAL}) \\times 10^{$EXP}\$"
        fi
    else 
        if [[ $EXP == 0 ]]; then
            echo "\$${SIGN}${VALUE_FINAL} \\pm ${ERROR_FINAL} \\pm ${SYS_FINAL} \$"
        else
            echo "\$(${SIGN}${VALUE_FINAL} \\pm ${ERROR_FINAL} \\pm ${SYS_FINAL}) \\times 10^{$EXP}\$"
        fi
    fi

}


# Normal (not scientific) notation
get_rounded() {

    # Get value and error
    VALUE=$1
    ERROR=$2
    SYS=""
    if [[ $# == 3 ]]; then
        SYS=$3
    fi

    # Get sign of value
    if [[ $(bc -l <<< "$VALUE < 0") == 1 ]]; then
        SIGN="-"
        VALUE=$(echo $VALUE | sed "s/-//")
    else
        SIGN=""
    fi

    # Check for zero value
    if [[ $(bc -l <<< "$VALUE == 0") == 1 ]]; then

        EXP=$(($(get_exponent $ERROR) + 1))
        PREC=1

    else 

        # Work out precision
        N_DP_VALUE=$(get_exponent $VALUE)
        N_DP_ERROR=$(get_exponent $ERROR)
        if [[ $(bc -l <<< "$N_DP_ERROR > $N_DP_VALUE") == 1 ]]; then
            EXP_VAL=$ERROR
            PREC=1
        else
            EXP_VAL=$VALUE
            PREC=$((N_DP_VALUE - N_DP_ERROR + 1))
        fi

        # Find exponent
        EXP=$(($(get_exponent $EXP_VAL) + 1))
    fi

    # Correct precision if first three digits are < 355
    FIRST_ERROR_DIGIT=$(echo $ERROR | sed 's/[\.0]*\([^\.0]\{3\}\).*/\1/')
    KEEP_ERR=1
    if [[ $FIRST_ERROR_DIGIT < 355 ]]; then
        PREC=$((PREC + 1))
        KEEP_ERR=2
    fi

    # Case where full number is printed
    if [[ $(bc -l <<< "$EXP >= 1") == 1 ]] && [[ $(bc -l <<< "$PREC == $EXP") == 1 ]]; then
        VALUE_FINAL=$(printf "%.0f" $VALUE)
        ERROR_FINAL=$(printf "%.0f" $ERROR)
        if [[ $SYS != "" ]]; then
            SYS_FINAL=$(printf "%.0f" $SYS)
        fi

    # Case where number is truncated
    elif [[ $(bc -l <<< "$EXP >= 1") == 1 ]] && [[ $(bc -l <<< "$PREC < $EXP") == 1 ]]; then

        # Significant digits
        VALUE_FINAL=$(printf "%.${PREC}g" $VALUE | sed 's/\([1-9]\)\.*\([0-9]*\).*/\1\2/')
        ERROR_FINAL=$(printf "%.${KEEP_ERR}g" $ERROR | sed 's/\([1-9]\)\.*\([0-9]*\).*/\1\2/')
        if [[ $SYS != "" ]]; then
            SYS_FINAL=$(printf "%.${KEEP_ERR}g" $SYS | sed 's/\([1-9]\)\.*\([0-9]*\).*/\1\2/')
        fi

        # Add extra zeros
        ZEROS=$(($EXP - ${#VALUE_FINAL}))
        ZEROS_ERR=$(($N_DP_ERROR + 1 - ${#ERROR_FINAL}))
        COUNT=0
        while [[ $COUNT -lt $ZEROS ]]; do
            VALUE_FINAL="${VALUE_FINAL}0"
            COUNT=$((COUNT + 1))
        done
        COUNT=0
        while [[ $COUNT -lt $ZEROS_ERR ]]; do
            ERROR_FINAL="${ERROR_FINAL}0"
            COUNT=$((COUNT + 1))
        done

    # Case with decimal places
    else 
        N_DP=$((PREC - EXP))
        VALUE_FINAL=$(printf "%.${N_DP}f" $VALUE)
        ERROR_FINAL=$(printf "%.${N_DP}f" $ERROR)
        SYS_FINAL=$(printf "%.${N_DP}f" $SYS)
    fi

    # Print
    if [[ $SYS == "" ]]; then
        echo ${SIGN}$VALUE_FINAL $ERROR_FINAL
    else 
        echo ${SIGN}$VALUE_FINAL $ERROR_FINAL $SYS_FINAL
    fi
}


# Print in normal format
n_no() {

    ROUNDED=$(get_rounded $*)
    if [[ $# == 3 ]]; then 
        awk '{print \$ $1 \pm $2 \pm $3 \$}' <<< $ROUNDED
    else 
        awk '{print \$ $1 \pm $2 \$}' <<< $ROUNDED
    fi
}


# Print in table format
tab_no() {

    # Get rounded numbers
    ROUNDED=$(get_rounded $*)
    SYS=""
    VAL=$(awk '{print $1}' <<< $ROUNDED)
    STAT=$(awk '{print $2}' <<< $ROUNDED)
    if [[ $# == 3 ]]; then
        SYS=$(awk '{print $3}' <<< $ROUNDED)
    fi

    # Add phantom + sign if needed
    if [[ $(bc -l <<< "$VAL >= 0") == 1 ]]; then
        VAL="\\phantom{+}$VAL"
    fi

    # Write in table format
    if [[ $SYS == "" ]]; then
        echo "& \$$VAL\$ & \$\\pm\$ & \$$STAT\$ \\\\"
    else 
        echo "& \$$VAL\$ & \$\\pm\$ & \$$STAT\$ & \$\\pm\$ & \$$SYS\$"
    fi
}
