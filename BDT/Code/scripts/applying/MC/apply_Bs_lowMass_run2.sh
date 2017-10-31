#PBS -l cput=1:59:00
#PBS -l walltime=1:59:00

# Set up
cd /home/pullen/analysis/B02DKstar/BDT/Code/
TUPLEDIR=/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs_lowMass/

# Apply to each of the run 2 MC samples
for particle in $(ls $TUPLEDIR); do
    for helicity in $(ls $TUPLEDIR/$particle); do
        FULLDIR=$TUPLEDIR/$particle/$helicity
        ./ApplyMVA Kpi_run2 $FULLDIR/2016_down/Total.root $FULLDIR/2016_down/Kpi_withBDTG.root "Tuple_Kpi/DecayTree"
        ./ApplyMVA Kpi_run2 $FULLDIR/2016_up/Total.root $FULLDIR/2016_up/Kpi_withBDTG.root "Tuple_Kpi/DecayTree"
    done
done

echo "Applied MVA to 2016 down"
