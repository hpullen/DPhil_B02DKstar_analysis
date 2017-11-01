#PBS -l cput=100:00:00
#PBS -l walltime=100:00:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/twoBody/2012_up/
./ApplyMVA Kpi_run2 $TUPLEDIR/Total.root $TUPLEDIR/Kpi_withBDTG.root "Tuple_Kpi/DecayTree"
echo "Applied MVA to 2012 up"
