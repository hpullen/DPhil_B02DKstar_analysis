j = Job(name='KK_2012_up',
        backend=Dirac(),
        application=DaVinci(version = "v41r3"),
        splitter=SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2012_KK.py'

BK = BKQuery(path=('//MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim08a/'
                   'Digi13/Trig0x409f0045/Reco14a/'
                   'Stripping20NoPrescalingFlagged/'
                   '11164021/ALLSTREAMS.DST'))

j.inputdata = BK.getDataset()
j.comment = '2012 MC KK up'
j.submit()
