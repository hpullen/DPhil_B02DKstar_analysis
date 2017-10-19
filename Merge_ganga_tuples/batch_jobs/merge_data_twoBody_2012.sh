#PBS -l cput=04:00:00
#PBS -l walltime=04:00:00

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

./merge.sh data/twoBody/2012_down
./merge.sh data/twoBody/2012_up
