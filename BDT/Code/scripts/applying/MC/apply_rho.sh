#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

# Set up
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho

# Apply to each of the run 2 MC samples
./ApplyMVA Kpi_run2 $TUPLEDIR/2012_down/Total.root $TUPLEDIR/2012_down/Kpi_withBDTG.root "TupleB0ToD0KPi_D0ToKPi/DecayTree"
./ApplyMVA Kpi_run2 $TUPLEDIR/2012_up/Total.root $TUPLEDIR/2012_up/Kpi_withBDTG.root "TupleB0ToD0KPi_D0ToKPi/DecayTree"

echo "Applied MVA to 2012 rho MC"
