#PBS -l cput=100:00:00
#PBS -l walltime=100:00:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/twoBody/2016_down/
./ApplyMVA Kpi_run2 $TUPLEDIR/Total.root $TUPLEDIR/piK_withBDTG.root "Tuple_piK/DecayTree"
echo "Applied MVA to 2016 down"
