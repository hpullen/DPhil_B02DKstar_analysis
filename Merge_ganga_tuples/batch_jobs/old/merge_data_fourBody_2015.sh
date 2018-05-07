#PBS -l cput=02:00:00
#PBS -l walltime=02:00:00

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

./merge.sh data/fourBody/2015_down
./merge.sh data/fourBody/2015_up
