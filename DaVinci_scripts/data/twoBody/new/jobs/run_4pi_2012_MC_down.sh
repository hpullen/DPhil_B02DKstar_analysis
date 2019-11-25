#!/usr/bin/env bash 
#PBS -j oe
#PBS -o /home/pullen/analysis/B02DKstar/DaVinci_scripts/data/twoBody/new/jobs/magDown.log
#PBS -q normal

export X509_USER_PROXY=${HOME}/.gridProxy
source /data/lhcb/sw/scripts/lbsetup-cvmfs.sh
cd /home/pullen/analysis/B02DKstar/DaVinci_scripts/data/twoBody/new/
lb-run -c best DaVinci/v39r1p6 gaudirun.py test_2012_4pi_MC.py

