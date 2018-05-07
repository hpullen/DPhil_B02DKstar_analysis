#PBS -l cput=02:00:00
#PBS -l walltime=02:00:00

BG_DIR=/data/lhcb/users/pullen/gangadir/job_output/MC/backgrounds

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

for bg_type in $(ls $BG_DIR); do
    if [[ $bg_type != "Bs_lowMass" ]] && [[ $bg_type != "lowMass" ]]; then
        for sample in $(ls ${BG_DIR}/${bg_type}); do
        ./merge.sh MC/backgrounds/${bg_type}/${sample}
    done
    fi
done

