snakemake --cluster-config cluster.json --cluster "qsub -q {cluster.queue} -e {cluster.logs} -o {cluster.logs}" --jobs 100
