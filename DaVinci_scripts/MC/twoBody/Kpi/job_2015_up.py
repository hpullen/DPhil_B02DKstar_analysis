j = Job(name='2015_mc_up',
        backend=Dirac(),
        application=DaVinci(version="v41r3"),
        splitter=SplitByFiles(filesPerJob=20, maxFiles=-1)
        )

j.application.optsfile = '2015_Bd_DKstar.py'

BK = BKQuery(path=('//MC/2015/Beam6500GeV-2015-MagUp-Nu1.6-25ns-Pythia8/'
                   'Sim09b/Trig0x411400a2/Reco15a/Turbo02/'
                   'Stripping24NoPrescalingFlagged/11164011/ALLSTREAMS.MDST'))

j.inputdata = BK.getDataset()
j.comment = '2015 signal mc, magup'
j.submit()
