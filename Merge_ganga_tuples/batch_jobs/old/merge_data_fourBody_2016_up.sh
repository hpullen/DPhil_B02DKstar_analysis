#PBS -l cput=50:00:00
#PBS -l walltime=70:00:00

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

./merge.sh data/fourBody/2016_up
