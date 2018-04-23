j = Job(name = '2015 data down')

#  myApp = prepareGaudiExec('DaVinci','v42r6p1', myPath='/home/pullen/cmtuser')
myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v42r6p1'
myApp.platform = 'x86_64-slc6-gcc62-opt'

j.application = myApp
j.application.options = ['2015_Bd_DKstar.py']

BK = BKQuery(path='//LHCb/Collision15/Beam6500GeV-VeloClosed-MagUp/Real Data/Reco15a/Stripping24r1/90000000/BHADRON.MDST')
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2015 data S24r1 down'

j.submit()
