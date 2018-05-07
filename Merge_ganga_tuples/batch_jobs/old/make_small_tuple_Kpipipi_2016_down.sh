#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/
./makeTuple.sh 2016 Kpipipi data fourBody down Tuple_Kpipipi/DecayTree
