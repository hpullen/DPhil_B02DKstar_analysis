#PBS -l cput=02:00:00
#PBS -l walltime=02:00:00

TWO_BODY_DIR=/data/lhcb/users/pullen/gangadir/job_output/MC/twoBody/
FOUR_BODY_DIR=/data/lhcb/users/pullen/gangadir/job_output/MC/fourBody/

cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/

for mode in $(ls $TWO_BODY_DIR); do
    for sample in $(ls ${TWO_BODY_DIR}/${mode}); do
        ./merge.sh MC/twoBody/${mode}/${sample}
    done
done

for mode in $(ls $FOUR_BODY_DIR); do
    for sample in $(ls ${FOUR_BODY_DIR}/${mode}); do
        if [[ $sample != "wrong_ptasy" ]]; then
            ./merge.sh MC/fourBody/${mode}/${sample}
        fi
    done
done
