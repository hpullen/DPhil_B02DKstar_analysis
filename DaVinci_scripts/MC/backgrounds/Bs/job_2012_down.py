j = Job(name = '2012 Bs MC down')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v41r2'

j.application = myApp
j.application.options = ['2012_Bd_DKstar.py']

BK = BKQuery(path=('//MC/2012/Beam4000GeV-2012-MagDown-Nu2.5-Pythia8/Sim08b/'
                   'Digi13/Trig0x409f0045/Reco14a/'
                   'Stripping20NoPrescalingFlagged/'
                   '13164051/ALLSTREAMS.DST'))
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2012 MC Bs down'

j.submit()
