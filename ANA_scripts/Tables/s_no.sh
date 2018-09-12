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

    # Correct DP if first digit is one or two
    FIRST_ERROR_DIGIT=$(echo $ERROR | sed 's/[\.0]*\([^\.0]\).*/\1/')
    if [[ $FIRST_ERROR_DIGIT < 3 ]]; then
        N_DP=$((N_DP + 1))
    fi

    # Get truncated values
    VALUE_FINAL=$(printf "%.${N_DP}f" $VALUE_CORR)
    ERROR_FINAL=$(printf "%.${N_DP}f" $ERROR_CORR)

    # Print in scientific notation
    if [[ $EXP == 0 ]]; then
        echo "\$${SIGN}${VALUE_FINAL} \\pm ${ERROR_FINAL}\$"
    else
        echo "\$(${SIGN}${VALUE_FINAL} \\pm ${ERROR_FINAL}) \\times 10^{$EXP}\$"
    fi

}


# Normal (not scientific) notation
n_no() {

    # Get value and error
    VALUE=$1
    ERROR=$2

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

    # Correct precision if first digit is one or two
    FIRST_ERROR_DIGIT=$(echo $ERROR | sed 's/[\.0]*\([^\.0]\).*/\1/')
    KEEP_ERR=1
    if [[ $FIRST_ERROR_DIGIT < 3 ]]; then
        PREC=$((PREC + 1))
        KEEP_ERR=2
    fi

    # Case where full number is printed
    if [[ $(bc -l <<< "$EXP >= 1") == 1 ]] && [[ $(bc -l <<< "$PREC == $EXP") == 1 ]]; then
        VALUE_FINAL=$(printf "%.0f" $VALUE)
        ERROR_FINAL=$(printf "%.0f" $ERROR)

    # Case where number is truncated
    elif [[ $(bc -l <<< "$EXP >= 1") == 1 ]] && [[ $(bc -l <<< "$PREC < $EXP") == 1 ]]; then

        # Significant digits
        VALUE_FINAL=$(printf "%.${PREC}g" $VALUE | sed 's/\([1-9]\)\.*\([0-9]*\).*/\1\2/')
        ERROR_FINAL=$(printf "%.${KEEP_ERR}g" $ERROR | sed 's/\([1-9]\)\.*\([0-9]*\).*/\1\2/')

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
    fi

    # Print
    echo "\$${SIGN}${VALUE_FINAL} \\pm ${ERROR_FINAL}\$"

}
