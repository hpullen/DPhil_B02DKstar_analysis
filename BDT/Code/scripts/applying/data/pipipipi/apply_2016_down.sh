#PBS -l cput=100:00:00
#PBS -l walltime=100:00:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/fourBody/2016_down/
./ApplyMVA pipipipi $TUPLEDIR/Total.root $TUPLEDIR/pipipipi_withBDTG.root
echo "Applied MVA to 2016 down"
