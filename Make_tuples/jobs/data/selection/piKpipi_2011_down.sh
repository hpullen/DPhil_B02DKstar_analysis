#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/Make_tuples/
./MakeSelectedTuple data 2011 down fourBody piKpipi
echo "Made tuple for 2011 piKpipi down"
