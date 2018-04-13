j = Job(name = '2011MonteCarlo2')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v41r2'

j.application = myApp
j.application.options = ['2011_Bd_DKstar.py']

BK = BKQuery(path='//MC/2011/Beam3500GeV-2011-MagUp-Nu2-Pythia8/Sim08e/Digi13/Trig0x40760037/Reco14a/Stripping20r1NoPrescalingFlagged/11164011/ALLSTREAMS.DST')
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2011 montecarlo magup, including MCTruth and backgroundcategory'

j.submit()