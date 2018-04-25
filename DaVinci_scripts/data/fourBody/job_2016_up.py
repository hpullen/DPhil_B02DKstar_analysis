j = Job(name = '2016 4body data up')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v42r6p1'
myApp.platform = 'x86_64-slc6-gcc62-opt'

j.application = myApp
j.application.options = ['2016_four_body.py']

BK = BKQuery(path='//LHCb/Collision16/Beam6500GeV-VeloClosed-MagUp/Real Data/Reco16/Stripping28r1/90000000/BHADRON.MDST')
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2016 4body data S28r1 up'

j.submit()
