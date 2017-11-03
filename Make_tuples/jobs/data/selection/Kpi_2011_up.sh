#PBS -l cput=1:59:00
#PBS -l walltime=1:59:00

cd /home/pullen/analysis/B02DKstar/Make_tuples/
./MakeSelectedTuple data 2011 up twoBody Kpi
echo "Made tuple for 2011 Kpi up"
