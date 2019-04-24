j = Job(name = '2017 data down')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v42r6p1'
myApp.platform = 'x86_64-slc6-gcc62-opt'

j.application = myApp
j.application.options = ['2017_Bd_DKstar.py']

BK = BKQuery(path='/LHCb/Collision17/Beam6500GeV-VeloClosed-MagDown/Real Data/Reco17/Stripping29r2/90000000/BHADRON.MDST')
j.inputdata = BK.getDataset()

j.outputfiles=[DiracFile('*.root'), LocalFile("*.xml")]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2017 data S29r2 down'

j.submit()
