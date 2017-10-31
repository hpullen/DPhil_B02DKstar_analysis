#PBS -l cput=1:59:00
#PBS -l walltime=1:59:00

# Set up
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/MC/fourBody/Kpipipi/

# Apply to each of the run 2 MC samples
./ApplyMVA Kpipipi_run2 $TUPLEDIR/2015_down/Total.root $TUPLEDIR/2015_down/Kpipipi_withBDTG.root "Tuple_K3pi/DecayTree"
./ApplyMVA Kpipipi_run2 $TUPLEDIR/2015_up/Total.root $TUPLEDIR/2015_up/Kpipipi_withBDTG.root "Tuple_K3pi/DecayTree"
./ApplyMVA Kpipipi_run2 $TUPLEDIR/2016_down/Total.root $TUPLEDIR/2016_down/Kpipipi_withBDTG.root "Tuple_K3pi/DecayTree"
./ApplyMVA Kpipipi_run2 $TUPLEDIR/2016_up/Total.root $TUPLEDIR/2016_up/Kpipipi_withBDTG.root "Tuple_K3pi/DecayTree"


echo "Applied MVA to 2016 down"
