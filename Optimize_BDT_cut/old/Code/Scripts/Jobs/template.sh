#!/usr/bin/env bash 
#PBS -l cput=1:59:00
#PBS -l walltime=1:59:00

# Template for a BDT study job
cd ~/analysis/B02DKstar/Fit_data_new/
./StudyBDT ID CUT_KPI CUT_KK CUT_PIPI CUT_KPIPIPI CUT_PIPIPIPI
