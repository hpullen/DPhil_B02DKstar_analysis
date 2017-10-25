#PBS -l cput=11:59:00
#PBS -l walltime=11:59:00

cd /home/pullen/analysis/B02DKstar/Make_tdownles/
./MakeTrainingTdownle data 2015 down twoBody pipi
echo "Made tdownle for 2015 pipi down"
