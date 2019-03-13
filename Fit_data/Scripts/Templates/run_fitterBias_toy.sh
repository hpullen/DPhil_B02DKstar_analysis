#/usr/bin/bash
#PBS -l cput=11:59:59
#PBS -l walltime=11:59:59

echo $PATH | sed 's/:/\n/g' > $HOME/path_script.txt
echo $LD_LIBRARY_PATH | sed 's/:/\n/g' > $HOME/ld_path_script.txt

cd /home/pullen/analysis/B02DKstar/Fit_data/
./FitterBias TOYNUMBER EXTRAOPTS
