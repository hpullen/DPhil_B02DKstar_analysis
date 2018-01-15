#/usr/bin/bash

# Submit many batch jobs
NAME=$1
for i in $(seq 1 1 250); do
    cp run_systematic.sh jobs/sys_${i}.sh
    sed -i "s/SYSNAME/${NAME}/" jobs/sys_${i}.sh
    sed -i "s/SYSNUMBER/${i}/" jobs/sys_${i}.sh
    qsub -q veryshort -e job_logs/ -o job_logs/ jobs/sys_${i}.sh
done
