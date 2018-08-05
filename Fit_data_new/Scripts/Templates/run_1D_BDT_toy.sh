#!/usr/bin/env bash 
#PBS -l cput=01:59:00
#PBS -l walltime=01:59:00

cd /home/pullen/analysis/B02DKstar/Fit_data_new/
./StudyBDTtoy OUTFILE piK_cut KK_cut pipi_cut piKpipi_cut pipipipi_cut
