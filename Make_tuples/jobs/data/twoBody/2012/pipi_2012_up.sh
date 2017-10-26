#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/Make_tuples/
./MakeTrainingTuple data 2012 up twoBody pipi
echo "Made tuple for 2012 pipi up"
