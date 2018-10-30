j = Job(name='2016_down_Kpi_mc')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v42r6p1'
myApp.platform = 'x86_64-slc6-gcc62-opt'

j.application = myApp
j.application.options = ['2016_Bd_DKstar.py']

BK = BKQuery(path=('//MC/2016/Beam6500GeV-2016-MagUp-Nu1.6-25ns-Pythia8/'
                   'Sim09b/Trig0x6138160F/Reco16/Turbo03/'
                   'Stripping26NoPrescalingFlagged/11164011/ALLSTREAMS.MDST'))
j.inputdata = BK.getDataset()

j.comment = '2016 Kpi signal mc ip'
j.backend = Dirac()
j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.submit()
