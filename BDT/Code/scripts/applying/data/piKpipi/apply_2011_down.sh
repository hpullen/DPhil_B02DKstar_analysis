#PBS -l cput=50:00:00
#PBS -l walltime=70:00:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/fourBody/2011_down/
./ApplyMVA Kpipipi $TUPLEDIR/Total.root $TUPLEDIR/piKpipi_withBDTG.root
echo "Applied MVA to 2011 down"
