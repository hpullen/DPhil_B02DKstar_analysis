#PBS -l cput=1:59:00
#PBS -l walltime=1:59:00

cd /home/pullen/analysis/B02DKstar/Fit_data/Code/

for Kpi_cut in seq 0.2 0.1 0.9; do
    for KK_cut in seq 0.2 0.1 0.9; do
        ./FitTwoBody 2011:2012:2015:2016 N Y $Kpi_cut 0.5 $KK_cut 0.5
    done 
done
