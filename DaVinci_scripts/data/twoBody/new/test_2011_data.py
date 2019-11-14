import sys,os
sys.path.append(os.getcwd())
from B2DKstar_davinci_options import setup_options
y = '11'
m = 'MagDown'
input_type = "MDST"
test_output = "test_2011_data.root"
test_files = ["/data/lhcb/users/pullen/DSTs/real_data/2011_21r1p2/00093829_00000001_1.bhadron.mdst"] # 2011 real data

setup_options(y, m, input_type, test=True, simulation=False,
        test_output=test_output, test_files=test_files,
        event_max=3000, fourBody_only=True)
