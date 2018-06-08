#/usr/bin/bash

echo $PATH | sed 's/:/\n/g' > $HOME/path_script.txt
echo $LD_LIBRARY_PATH | sed 's/:/\n/g' > $HOME/ld_path_script.txt

cd /home/pullen/analysis/B02DKstar/Fit_data_new/
./FitterBias 1
