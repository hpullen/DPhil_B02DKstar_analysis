#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/Make_tuples/
./MakeTrainingTuple data 2015 down fourBody Kpipipi
echo "Made tuple for 2015 Kpipipi down"
