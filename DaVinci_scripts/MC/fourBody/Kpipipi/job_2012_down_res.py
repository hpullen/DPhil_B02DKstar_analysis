j = Job(name = '2012 resonant Kpipipi MC down')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v42r6p1/'

j.application = myApp
j.application.options = ['2012_K3pi.py']

BK = BKQuery(path=('/MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim08a/Digi13/Trig0x409f0045/Reco14a/Stripping20NoPrescalingFlagged/11266001/ALLSTREAMS.DST'))
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2012 resonant Kpipipi MC down'

j.submit()
