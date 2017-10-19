j = Job(name = '2011 up',
        backend = Dirac(),
        application = DaVinci(version = "v41r3"),
        splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2011_Bd_DKstar.py'

BK = BKQuery(path='/LHCb/Collision11/Beam3500GeV-VeloClosed-MagUp/Real Data/Reco14/Stripping21r1/90000000/BHADRON.MDST')

j.inputdata = BK.getDataset()
j.comment = '2011 data magup'
j.submit()
