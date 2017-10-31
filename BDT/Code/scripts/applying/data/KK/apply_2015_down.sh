#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/twoBody/2015_down/
./ApplyMVA KK_run2 $TUPLEDIR/Total.root $TUPLEDIR/KK_withBDTG.root "Tuple_KK/DecayTree"
echo "Applied MVA to 2015 down"
