#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/Fit_data/Code/

for Kpi_cut in $(seq -0.3 0.1 0.9); do
    for pipi_cut in $(seq -0.3 0.1 0.9); do
        ./FitTwoBody 2011:2012:2015:2016 N Y $Kpi_cut $Kpi_cut 0.5 $pipi_cut N
    done 
done
