export ROOTSYS=/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_84/ROOT/6.06.02/x86_64-slc6-gcc49-opt
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ROOTSYS}/lib
export PATH=$PATH:$ROOTSYS/bin
cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/;
./makeTuple.sh 2011 Kpi data twoBody down Tuple_Kpi/DecayTree
