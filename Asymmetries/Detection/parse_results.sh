#!/usr/bin/env bash

# Extract detection asymmetry results from output files
for OUTPUT in kpipi kspi; do
    FILE=Results/A_${OUTPUT}.param
    if [[ -f $FILE ]]; then rm $FILE; fi
    for YEAR in 201{1,2,5,6}_{up,down}; do
        VAL=$(awk 'NR==1{print $1}' Results/D0/$YEAR/output_${OUTPUT}.txt)
        ERROR=$(awk 'NR==1{print $2}' Results/D0/$YEAR/output_${OUTPUT}.txt)
        echo $YEAR $VAL $ERROR >> $FILE
    done
done

# Calculate averages
./../../Efficiencies/Scripts/AverageEfficiency Results/both.param -i\
    A_Kpipi Results/A_kpipi.param A_Kspi Results/A_kspi.param
./../../Efficiencies/Scripts/AverageEfficiency Results/both_run1.param -i\
    A_Kpipi Results/A_kpipi.param A_Kspi Results/A_kspi.param --run1
./../../Efficiencies/Scripts/AverageEfficiency Results/both_run2.param -i\
    A_Kpipi Results/A_kpipi.param A_Kspi Results/A_kspi.param --run2

# Calculate A_Kpi = A_Kpipi - A_Kspi + A_Ks
A_Ks=$(awk '{print $2}' Results/A_ks.param)
A_Kpipi=$(awk '/A_Kpipi/{print $2}' Results/both.param)
A_Kspi=$(awk '/A_Kspi/{print $2}' Results/both.param)
echo "Calculating A_Kpi = A_Kpipi - A_Kspi + A_Ks"
echo "                  = ($A_Kpipi - $A_Kspi + $A_Ks)%"
A_Kpi=$(bc <<< "$A_Kpipi - $A_Kspi + $A_Ks")

# Calculate uncertainty
ERR_A_Ks=$(awk '{print $3}' Results/A_ks.param)
ERR_A_Kpipi=$(awk '/A_Kpipi/{print $3}' Results/both.param)
ERR_A_Kspi=$(awk '/A_Kspi/{print $3}' Results/both.param)
ERR_A_Kpi=$(bc -l <<< "sqrt(${ERR_A_Ks}^2 + ${ERR_A_Kpipi}^2 + ${ERR_A_Kspi}^2)")
echo "                  = ($A_Kpi +/- $ERR_A_Kpi)%"

# Save to file
REAL_A_Kpi=$(bc -l <<< "$A_Kpi/100")
REAL_ERR_A_Kpi=$(bc -l <<< "$ERR_A_Kpi/100")
echo "A_Kpi $REAL_A_Kpi $REAL_ERR_A_Kpi" > Results/A_Kpi.param



