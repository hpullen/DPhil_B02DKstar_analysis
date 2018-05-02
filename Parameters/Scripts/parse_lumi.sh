#!/usr/bin/env bash 
INFILE=../lumi_calculated.param
TOTAL_OUT=../luminosity.param
PROP_OUT=../polarity.param
if [[ -f $TOTAL_OUT ]]; then rm $TOTAL_OUT; fi
echo "Up Down" | awk '{printf "%s %6s %6s\n", "    ", $1, $2}' > $PROP_OUT

# Print total luminosity per year
for YEAR in 2011 2012 2015 2016; do

    # Get luminosities
    LUMI_UP=$(grep "^${YEAR}_up" $INFILE | awk '{print $2}')
    ERR_UP=$(grep "^${YEAR}_up" $INFILE | awk '{print $3}')
    LUMI_DOWN=$(grep "^${YEAR}_down" $INFILE | awk '{print $2}')
    ERR_DOWN=$(grep "^${YEAR}_down" $INFILE | awk '{print $3}')

    # Write total luminosity to file
    LUMI_TOTAL=$(echo "($LUMI_UP + $LUMI_DOWN)/1000" | bc -l | awk '{printf "%.3f", $0}')
    ERR_TOTAL=$(echo "sqrt($ERR_UP^2 + $ERR_DOWN^2)/1000" | bc -l | awk '{printf "%.3f", $0}')
    echo "${YEAR} ${LUMI_TOTAL} ${ERR_TOTAL}" >> $TOTAL_OUT

    # Write proportion of polarities to file
    PROP_UP=$(echo "100*$LUMI_UP/($LUMI_UP + $LUMI_DOWN)" | bc -l | awk '{printf "%.1f", $0}')
    PROP_DOWN=$(echo "100*$LUMI_DOWN/($LUMI_UP + $LUMI_DOWN)" | bc -l | awk '{printf "%.1f", $0}')
    echo "$YEAR $PROP_UP $PROP_DOWN" | awk '{printf "%s %6s %6s\n", $1, $2, $3}' >> $PROP_OUT

done
