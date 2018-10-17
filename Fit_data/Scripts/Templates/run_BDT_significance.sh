#!/usr/bin/env bash 
#PBS -l cput=1:59:59
#PBS -l walltime=1:59:59

cd /home/pullen/analysis/B02DKstar/Fit_data/
./Significance OUTFILE MODE CUT
