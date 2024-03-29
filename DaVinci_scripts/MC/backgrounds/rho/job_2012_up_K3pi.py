j = Job(name = 'rho_K3pi_12u')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v42r6p1/'

j.application = myApp
j.application.options = ['2012_K3pi.py']

BK = BKQuery(path=('//MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim09b/Trig0x409f0045/Reco14c/Stripping21NoPrescalingFlagged/11166054/ALLSTREAMS.DST'))
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2012 rho Kpipipi MC up'

j.submit()
