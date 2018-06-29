#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/data/fourBody/2016_down/
./ApplyMVA Kpipipi $TUPLEDIR/Total.root $TUPLEDIR/piKpipi_withBDTG.root
echo "Applied MVA to 2016 down, third set of events"
