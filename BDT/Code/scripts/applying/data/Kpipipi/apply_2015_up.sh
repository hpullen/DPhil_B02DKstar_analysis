#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/fourBody/2015_up/
./ApplyMVA Kpipipi $TUPLEDIR/Total.root $TUPLEDIR/Kpipipi_withBDTG.root
echo "Applied MVA to 2015 up"
