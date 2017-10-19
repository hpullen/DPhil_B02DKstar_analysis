j = Job(name = '2012MonteCarlo2',
        backend = Dirac(),
        application = DaVinci(version = "v41r3"),
        splitter = SplitByFiles(filesPerJob = 20, maxFiles = -1)
        )

j.application.optsfile = '2012_Bd_DKstar.py'

BK = BKQuery(path='//MC/2012/Beam4000GeV-2012-MagDown-Nu2.5-Pythia8/Sim08e/Digi13/Trig0x409f0045/Reco14a/Stripping20NoPrescalingFlagged/11164011/ALLSTREAMS.DST')

j.inputdata = BK.getDataset()
j.comment = '2012 montecarlo magdown, including MCTruth and backgroundcategory'
j.submit()
