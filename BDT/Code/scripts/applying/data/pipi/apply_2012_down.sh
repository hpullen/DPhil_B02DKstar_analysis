#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/twoBody/2012_down/
./ApplyMVA pipi_run2 $TUPLEDIR/Total.root $TUPLEDIR/pipi_withBDTG.root "Tuple_pipi/DecayTree"
echo "Applied MVA to 2012 down"
