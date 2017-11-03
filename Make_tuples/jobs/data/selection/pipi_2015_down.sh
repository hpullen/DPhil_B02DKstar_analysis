#PBS -l cput=1:59:00
#PBS -l walltime=1:59:00

cd /home/pullen/analysis/B02DKstar/Make_tuples/
./MakeSelectedTuple data 2015 down twoBody pipi
echo "Made tuple for 2015 pipi down"
