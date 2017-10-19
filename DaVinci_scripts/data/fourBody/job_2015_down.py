j = Job(name='2015_4body_down')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v41r2'

j.application = myApp
j.application.options = ['2015_four_body.py']

BK = BKQuery(path='/LHCb/Collision15/Beam6500GeV-VeloClosed-MagDown/Real Data/Reco15a/Stripping24/90000000/BHADRON.MDST')
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = 'Corrected ptasy'
j.submit()
