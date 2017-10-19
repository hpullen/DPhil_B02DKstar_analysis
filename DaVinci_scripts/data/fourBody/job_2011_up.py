j = Job(name='2011_4body_up')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v41r2'

j.application = myApp
j.application.options = ['2011_four_body.py']

BK = BKQuery(path='/LHCb/Collision11/Beam3500GeV-VeloClosed-MagUp/Real Data/Reco14/Stripping21r1/90000000/BHADRON.MDST')
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = 'data11_4b_up_ptasy'
j.submit()
