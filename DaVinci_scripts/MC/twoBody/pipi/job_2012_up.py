j = Job(name = '2012_MC_pipi_up')

myApp = GaudiExec()
myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v41r2'

j.application = myApp
j.application.options = ['2012_pipi.py']

BK = BKQuery(path=('//MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim08a/'
                   'Digi13/Trig0x409f0045/Reco14a/'
                   'Stripping20NoPrescalingFlagged/'
                   '11164031/ALLSTREAMS.DST'))
j.inputdata = BK.getDataset()

j.outputfiles=[LocalFile('*.root')]

j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
j.backend = Dirac()
j.comment = '2012 pipi MC up'
j.submit()
