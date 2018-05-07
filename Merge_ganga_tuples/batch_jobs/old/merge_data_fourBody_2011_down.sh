#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

./merge.sh data/fourBody/2011_down
