#/usr/bin/bash

# Submit many batch jobs
NAME="tight_pid_twoBody_only"
for i in $(seq 1 1 50); do
    cp run_significance_toys.sh significance_jobs/toys_${i}.sh
    sed -i "s/TOYNAME/${NAME}/" significance_jobs/toys_${i}.sh
    sed -i "s/TOYNUMBER/${i}/" significance_jobs/toys_${i}.sh
    qsub -q veryshort -e job_logs/ -o job_logs/ significance_jobs/toys_${i}.sh
done
