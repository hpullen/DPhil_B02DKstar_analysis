#!/usr/bin/env bash
# OUTPUT=/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Results/n_bookkeeping.txt
# if [[ -f $OUTPUT ]]; then
    # rm $OUTPUT
# fi
# for mode in Kpi KK pipi; do
    # for year in 2011 2012 2015 2016; do
        # for mag in up down; do
            # if [[ $year == "2011" && $mode != "Kpi" ]]; then
                # echo "Ignoring 2011 for mode != Kpi"
            # else
                # SUM=0
                # JOBDIR="$GANGADIR/job_output/MC/twoBody/${mode}/${year}_${mag}/"
                # for file in ${JOBDIR}/*/output/*.log; do
                    # ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9]\+")
                    # SUM=$((${SUM} + ${ADD}))
                # done
                # echo Sum for $mode $year $mag is $SUM
                # echo $mode $year $mag $SUM >> $OUTPUT
            # fi
        # done
    # done
# done

# for mag in up down; do
    # for year in 2012 2016; do
        # SUM=0
        # mode=Kpipipi
        # JOBDIR="$GANGADIR/job_output/MC/fourBody/${mode}/${year}_${mag}/"
        # for file in ${JOBDIR}/*/output/*.log; do
            # ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9][0-9]\+")
            # SUM=$((${SUM} + ${ADD}))
        # done
        # echo Sum for $mode $year $mag is $SUM
        # echo $mode $year $mag $SUM >> $OUTPUT
    # done
# done

# for mag in up down; do
    # SUM=0
    # mode=pipipipi
    # year=2016
    # JOBDIR="$GANGADIR/job_output/MC/fourBody/${mode}/${year}_${mag}/"
    # for file in ${JOBDIR}/*/output/*.log; do
        # ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9][0-9]\+")
        # SUM=$((${SUM} + ${ADD}))
    # done
    # echo Sum for $mode $year $mag is $SUM
    # echo $mode $year $mag $SUM >> $OUTPUT
# done

# LOWMASS_OUTPUT="/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Results/n_bookkeeping_lowMass.txt"
# if [[ -f $LOWMASS_OUTPUT ]]; then
    # rm $LOWMASS_OUTPUT
# fi
# for mag in up down; do
    # for particle in gamma pi; do
        # for helicity in 010 100 001; do
            # for year in 2012 2016; do
                # SUM=0
                # JOBDIR="$GANGADIR/job_output/MC/backgrounds/lowMass/${particle}/${helicity}/${year}_${mag}/"
                # for file in ${JOBDIR}/*/output/*.log; do
                    # ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9][0-9]\+")
                    # SUM=$((${SUM} + ${ADD}))
                # done
                # echo Low mass sum for $particle $helicity $mag is $SUM
                # echo $particle $helicity $year $mag $SUM >> "$LOWMASS_OUTPUT"
            # done
        # done
    # done
# done

# RHO_OUTPUT="/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Results/n_bookkeeping_rho.txt"
# if [[ -f $RHO_OUTPUT ]]; then
    # rm $RHO_OUTPUT
# fi
# for mag in up down; do
    # for year in 2012 2016; do
        # SUM=0
        # JOBDIR="$GANGADIR/job_output/MC/backgrounds/rho/${year}_${mag}/"
        # for file in ${JOBDIR}/*/output/*.log; do
            # ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9][0-9]\+")
            # SUM=$((${SUM} + ${ADD}))
        # done
        # echo Rho sum for $mag is $SUM
        # echo $year $mag $SUM >> "$RHO_OUTPUT"
    # done
# done

# RHO_OUTPUT="/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Results/n_bookkeeping_rho_Kpipipi.txt"
# if [[ -f $RHO_OUTPUT ]]; then
    # rm $RHO_OUTPUT
# fi
# for mag in up down; do
    # for year in 2012 2016; do
        # SUM=0
        # JOBDIR="$GANGADIR/job_output/MC/backgrounds/rho_Kpipipi/${year}_${mag}/"
        # for file in ${JOBDIR}/*/output/*.log; do
            # ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9][0-9]\+")
            # SUM=$((${SUM} + ${ADD}))
        # done
        # echo Rho sum for $mag is $SUM
        # echo $year $mag $SUM >> "$RHO_OUTPUT"
    # done
# done

# LAMBDA_OUTPUT="/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Results/n_bookkeeping_lambda_K.txt"
# if [[ -f $LAMBDA_OUTPUT ]]; then
    # rm $LAMBDA_OUTPUT
# fi
# for mag in up down; do
    # SUM=0
    # JOBDIR="$GANGADIR/job_output/MC/backgrounds/Lambda_b/2016_${mag}/"
    # for file in ${JOBDIR}/*/output/*.log; do
        # ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9][0-9]\+")
        # SUM=$((${SUM} + ${ADD}))
    # done
    # echo Lambda sum for $mag is $SUM
    # echo 2016 $mag $SUM >> "$LAMBDA_OUTPUT"
# done

# BS_OUTPUT=/home/pullen/analysis/B02DKstar/Efficiencies/Selection/Results/n_bookkeeping_Bs.txt
# for year in 2011 2012 2015 2016; do
    # for mag in up down; do
        # SUM=0
        # JOBDIR="$GANGADIR/job_output/MC/backgrounds/Bs/${mode}/${year}_${mag}/"
        # for file in ${JOBDIR}/*/output/*.log; do
            # ADD=$(grep "SUCCESS.*events processed" $file | grep -o "[0-9]\+")
            # SUM=$((${SUM} + ${ADD}))
        # done
        # echo Sum for $mode $year $mag is $SUM
        # echo $mode $year $mag $SUM >> $BS_OUTPUT
    # done
# done
