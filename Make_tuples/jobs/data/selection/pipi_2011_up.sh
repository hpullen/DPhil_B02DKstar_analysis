#PBS -l cput=1:59:00
#PBS -l walltime=1:59:00

cd /home/pullen/analysis/B02DKstar/Make_tuples/
./MakeSelectedTuple data 2011 up twoBody pipi
echo "Made tuple for 2011 pipi up"
