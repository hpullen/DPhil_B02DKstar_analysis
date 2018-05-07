#PBS -l cput=04:00:00
#PBS -l walltime=04:00:00

BG_DIR=/data/lhcb/users/pullen/gangadir/job_output/MC/backgrounds

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

i=0
for lowmass in "lowMass" "Bs_lowMass"; do
    for particle in $(ls ${BG_DIR}/${lowmass}); do
        for helicity in $(ls ${BG_DIR}/${lowmass}/${particle}); do
            for sample in $(ls ${BG_DIR}/${lowmass}/${particle}/${helicity}); do
                ./merge.sh MC/backgrounds/${lowmass}/${particle}/${helicity}/${sample}
            done
        done
    done
done
echo $i
