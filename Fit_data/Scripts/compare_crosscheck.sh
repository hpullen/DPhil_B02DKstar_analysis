#!/usr/bin/env bash 

# Check for outputs
DIR=/home/pullen/analysis/B02DKstar/Fit_data/
CCFILE=$DIR/Output/crossCheck.txt
if [[ ! -f $CCFILE ]]
then
    echo "Run cross check first! (./CrossCheck)"
fi
FITFILE=$DIR/Output/fitData.txt
if [[ ! -f $FITFILE ]]
then
    echo "Run data fit first! (./FitData)"
fi

# Get floating parameters from cross check
OUTPUT=$DIR/Output/crossCheck_vs_fitData.txt
sed -n '/Floating Parameter/,/^$/p' $CCFILE | sed -n '/pdf_params/,$p' | \
    sed 's/^ \+pdf_params_//' | awk '{print $1, $3}' > ${OUTPUT}.tmp
sed -i '$d' ${OUTPUT}.tmp

# Find corresponding parameters in data fit
if [[ -f $OUTPUT ]]; then rm $OUTPUT; fi
while read LINE; do
    NAME=$(echo $LINE | awk '{print $1}') 
    VAL=$(echo $LINE | awk '{print $2}' )
    VAL_BC=$(echo $VAL | sed 's/e+\([0-9]\+\)/*10^\1/' |\
        sed 's/e-\([0-9]\+\)/*10^-\1/')
    FITVAL=$(grep "^$NAME:" $FITFILE | awk '{print $2}')
    if [[ -z $FITVAL ]]; then continue; fi
    FITVAL_BC=$(echo $FITVAL | sed 's/e+\([0-9]\+\)/*10^\1/' |\
        sed 's/e-\([0-9]\+\)/*10^-\1/')
    DIFF=$(bc -l <<< "($FITVAL_BC - $VAL_BC)/($FITVAL_BC + $VAL_BC)")
    DIFF=$(bc -l <<< "scale=4; $DIFF/1" | sed 's/^\./0\./; s/^-\./-0\./')
    echo $NAME $FITVAL $VAL $DIFF >> $OUTPUT
done < ${OUTPUT}.tmp
rm ${OUTPUT}.tmp

# Sort file
cat $OUTPUT | sort -k4nr | column -t >> ${OUTPUT}.tmp
mv -f ${OUTPUT}.tmp $OUTPUT




