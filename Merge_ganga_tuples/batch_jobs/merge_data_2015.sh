#PBS -l cput=04:00:00
#PBS -l walltime=04:00:00

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

./merge.sh data/twoBody/2015_down
./merge.sh data/twoBody/2015_up
./merge.sh data/fourBody/2015_down
./merge.sh data/fourBody/2015_up
