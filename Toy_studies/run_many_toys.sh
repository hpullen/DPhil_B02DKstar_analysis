#/usr/bin/bash

# Submit many batch jobs, each running 2 toys
for i in $(seq 1 1 100); do
    qsub -q veryshort -e job_logs/ -o job_logs/ run_toys.sh
done
