# Script used to submit ganga jobs for data processing
import sys

#  jobs = ['gamma_001', 'gamma_010', 'gamma_100', 'pi_001', 'pi_010', 'pi_100']
#  jobs = ['pi_001', 'pi_010', 'pi_100']
#  mags  = [ 'Down', 'Up' ]

eventTypes = { 
        'gamma_001' : '11164201', 
        'gamma_010' : '11164202', 
        'gamma_100' : '11164203',
        'pi_001'    : '11164440',
        'pi_010'    : '11164441',
        'pi_100'    : '11164442'
        }

DaVinci_dir = '/home/pullen/cmtuser'
DaVinci_version = 'v44r6'
options_dir = "/home/pullen/analysis/B02DKstar/DaVinci_scripts/MC/backgrounds/lowMass_rho/"
options = {
    "Down" : "2016_Bd_DKstar_down.py",
    "Up"   : "2016_Bd_DKstar_up.py",
    }

#  for j in jobs:
    #  for m in mags:
j = sys.argv[1]
m = sys.argv[2]

# Print info
job_name = 'D*rho_{}_Mag{}'.format(j, m)
print 'Preparing job:', job_name

# General setup
bk_path = '//MC/2016/Beam6500GeV-2016-Mag{}-Nu1.6-25ns-Pythia8/'\
           'Sim09c/Trig0x6138160F/Reco16/Turbo03/'\
           'Stripping28r1NoPrescalingFlagged/{}/'\
           'ALLSTREAMS.MDST'.format(m, eventTypes[j])
BK = BKQuery(path=bk_path)

j = Job(name = job_name,
        backend = Dirac(),
        application = GaudiExec(),
        splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1),
        outputfiles = [
            DiracFile('*.root'),
            LocalFile('*.xml')]
        )

j.application.options   = ['{}/{}'.format(options_dir, options[m])]
j.application.directory = '{}/DaVinciDev_{}'.format(DaVinci_dir, DaVinci_version)
j.inputdata = BK.getDataset()
j.comment = 'MC: {}'.format(job_name)
j.submit()

