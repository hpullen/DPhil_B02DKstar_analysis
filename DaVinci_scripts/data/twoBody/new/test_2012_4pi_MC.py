import sys,os
sys.path.append(os.getcwd())
from B2DKstar_withRestrip_options import setup_options
y = '12'
m = 'MagDown'
input_type = "DST"
test_output = "test_output/test_4pi_2012_MC.root"
test_files = ["/data/lhcb/users/pullen/DSTs/MC/4body/4pi_2012.dst"]
conddb = 'sim-20160321-2-vc-md100'
dddb = 'dddb-20150928'

setup_options(y, m, input_type, test=True, simulation=True,
        test_output=test_output, test_files=test_files,
        event_max=3000, conddb=conddb, dddb=dddb)
