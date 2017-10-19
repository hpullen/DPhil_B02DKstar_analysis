j = Job(name='2012_4body_down')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v41r2'

j.application = myApp
j.application.options = ['2012_four_body.py']

BK = BKQuery(path='/LHCb/Collision12/Beam4000GeV-VeloClosed-MagDown/Real Data/Reco14/Stripping21/90000000/BHADRON.MDST')
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = 'data12_4b_down_ptasy'
j.submit()
