j = Job(name = '2015_down',
        backend = Dirac(),
        application = DaVinci(version = "v41r3"),
        splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2015_Bd_DKstar.py'

BK = BKQuery(path='/LHCb/Collision15/Beam6500GeV-VeloClosed-MagDown/Real Data/Reco15a/Stripping24/90000000/BHADRON.MDST')

j.inputdata = BK.getDataset()
j.comment = '2015 data magdown KK/pipi only'
j.submit()
