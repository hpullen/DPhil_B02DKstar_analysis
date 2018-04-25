j = Job(name = '2015 4body data down')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v42r6p1'
myApp.platform = 'x86_64-slc6-gcc62-opt'

j.application = myApp
j.application.options = ['2015_four_body.py']

BK = BKQuery(path='//LHCb/Collision15/Beam6500GeV-VeloClosed-MagDown/Real Data/Reco15a/Stripping24r1/90000000/BHADRON.MDST')
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2015 4body data S24r1 down'

j.submit()
