#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

# Set up
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/

# Apply to each of the run 2 MC samples
./ApplyMVA Kpi_run2 $TUPLEDIR/2015_down/Total.root $TUPLEDIR/2015_down/Kpi_withBDTG.root "Tuple_Kpi/DecayTree"
./ApplyMVA Kpi_run2 $TUPLEDIR/2015_up/Total.root $TUPLEDIR/2015_up/Kpi_withBDTG.root "Tuple_Kpi/DecayTree"
./ApplyMVA Kpi_run2 $TUPLEDIR/2016_down/Total.root $TUPLEDIR/2016_down/Kpi_withBDTG.root "Tuple_Kpi/DecayTree"
./ApplyMVA Kpi_run2 $TUPLEDIR/2016_up/Total.root $TUPLEDIR/2016_up/Kpi_withBDTG.root "Tuple_Kpi/DecayTree"


echo "Applied MVA to 2016 down"
