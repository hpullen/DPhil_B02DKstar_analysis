#PBS -l cput=50:00:00
#PBS -l walltime=70:00:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/fourBody/2015_down/
./ApplyMVA Kpipipi_run2 $TUPLEDIR/Total.root $TUPLEDIR/piKpipi_withBDTG.root "Tuple_piKpipi/DecayTree"
echo "Applied MVA to 2015 down"