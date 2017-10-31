#PBS -l cput=1:59:00
#PBS -l walltime=1:59:00

# Set up
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/MC/fourBody/pipipipi/

# Apply to each of the run 2 MC samples
./ApplyMVA pipipipi_run2 $TUPLEDIR/2015_down/Total.root $TUPLEDIR/2015_down/pipipipi_withBDTG.root "Tuple_4pi/DecayTree"
./ApplyMVA pipipipi_run2 $TUPLEDIR/2015_up/Total.root $TUPLEDIR/2015_up/pipipipi_withBDTG.root "Tuple_4pi/DecayTree"
./ApplyMVA pipipipi_run2 $TUPLEDIR/2016_down/Total.root $TUPLEDIR/2016_down/pipipipi_withBDTG.root "Tuple_4pi/DecayTree"
./ApplyMVA pipipipi_run2 $TUPLEDIR/2016_up/Total.root $TUPLEDIR/2016_up/pipipipi_withBDTG.root "Tuple_4pi/DecayTree"


echo "Applied MVA to 2016 down"
