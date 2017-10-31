#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/twoBody/2011_up/
./ApplyMVA Kpi_run2 $TUPLEDIR/Total.root $TUPLEDIR/Kpi_withBDTG.root "Tuple_Kpi/DecayTree"
echo "Applied MVA to 2011 up"
