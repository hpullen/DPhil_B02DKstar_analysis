#!/usr/bin/env bash 
#PBS -l cput=11:59:59
#PBS -l walltime=11:59:59

cd /home/pullen/analysis/B02DKstar/Fit_data/
./StudyBDT2 OUTFILE Kpi_cut KK_cut pipi_cut Kpipipi_cut pipipipi_cut
