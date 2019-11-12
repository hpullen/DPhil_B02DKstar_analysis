import sys,os
sys.path.append(os.getcwd())
from B2DKstar_davinci_options import setup_options
y = '18'
m = 'MagDown'
simulation = True
evt_type = '11164011'
stream = 'AllStreams'
dddb = 'dddb-20170721-3'
conddb = 'sim-20190430-vc-md100'
input_type = "MDST"
test_output = "test_MC.root"
test_files = [ "/data/lhcb/users/pullen/DSTs/MC/Signal/Kpi_2018/00096200_00000001_7.AllStreams.mdst" ]
test = True

setup_options(y, m, input_type, test=test, simulation=simulation,
        event_type=evt_type, conddb=conddb, dddb=dddb, stream=stream,
        test_output=test_output, test_files=test_files)
