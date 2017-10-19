j = Job(name='2016_up')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v41r2'

j.application = myApp
j.application.options = ['2016_Bd_DKstar.py']

BK = BKQuery(path='/LHCb/Collision16/Beam6500GeV-VeloClosed-MagUp/Real Data/Reco16/Stripping28/90000000/BHADRON.MDST')
j.inputdata = BK.getDataset()

j.outputfiles = [LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2016 data up gaudiexec'
j.submit()
