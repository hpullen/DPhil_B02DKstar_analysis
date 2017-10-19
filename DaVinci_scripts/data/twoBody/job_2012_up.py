j = Job(name = '2012 up',
        backend = Dirac(),
        application = DaVinci(version = "v41r3"),
        splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2012_Bd_DKstar.py'

BK = BKQuery(path='/LHCb/Collision12/Beam4000GeV-VeloClosed-MagUp/Real Data/Reco14/Stripping21/90000000/BHADRON.MDST')

j.inputdata = BK.getDataset()
j.comment = '2012 data magup'
j.submit()
