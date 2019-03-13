OUTFILE=large_biases${1}.param
if [[ -f $OUTFILE ]]; then rm $OUTFILE; fi
while IFS='' read -r LINE || [[ -n "$LINE" ]]; do
    PAR=$(echo $LINE | awk '{print $1}')
    MEAN=$(echo $LINE | awk '{print $2}')
    ERR=$(echo $LINE | awk '{print $3}')
    RATIO=$(bc -l <<< "sqrt($MEAN/$ERR * $MEAN/$ERR)")
    if [[ $(bc -l <<< "$RATIO >= 3") == 1 ]]; then
        echo $LINE >> $OUTFILE
    fi
done < biases${1}.param

