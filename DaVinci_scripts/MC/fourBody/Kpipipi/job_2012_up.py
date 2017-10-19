j = Job(name='K3pi_2012_up',
        backend=Dirac(),
        application=DaVinci(version = "v41r3"),
        splitter=SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2012_K3pi.py'

BK = BKQuery(path=('//MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim09b/Trig0x409f0045/Reco14c/Stripping21NoPrescalingFlagged/11166053/ALLSTREAMS.DST'))

j.inputdata = BK.getDataset()
j.comment = 'MC2012_Kpipipi'
j.submit()
