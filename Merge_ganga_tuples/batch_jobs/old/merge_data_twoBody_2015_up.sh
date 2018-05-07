#PBS -l cput=30:00:00
#PBS -l walltime=50:00:00

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

./merge.sh data/twoBody/2015_up
