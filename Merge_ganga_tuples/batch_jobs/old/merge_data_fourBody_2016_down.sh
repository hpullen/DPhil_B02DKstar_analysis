#PBS -l cput=75:00:00
#PBS -l walltime=100:00:00

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

./merge.sh data/fourBody/2016_down
