#PBS -l cput=1:59:00
#PBS -l walltime=1:59:00

cd /home/pullen/analysis/B02DKstar/Fit_data/Code/

for piK_cut in seq 0.2 0.1 0.9; do
    for pipi_cut in seq 0.2 0.1 0.9; do
        ./FitTwoBody 2011:2012:2015:2016 N Y 0.5 $piK_cut 0.5 $pipi_cut
    done 
done
