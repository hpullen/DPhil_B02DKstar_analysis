#PBS -l cput=08:00:00
#PBS -l walltime=08:00:00

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

./merge.sh data/twoBody/2016_down
./merge.sh data/twoBody/2016_up
