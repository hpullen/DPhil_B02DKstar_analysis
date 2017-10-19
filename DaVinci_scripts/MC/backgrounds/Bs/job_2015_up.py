j = Job(name='Bs_2015_up',
        backend=Dirac(),
        application=DaVinci(version="v41r3"),
        splitter=SplitByFiles(filesPerJob=10, maxFiles=-1)
        )

j.application.optsfile = '2015_Bd_DKstar.py'

BK = BKQuery(path=('//MC/2015/Beam6500GeV-2015-MagUp-Nu1.6-25ns-Pythia8/'
                   'Sim09b/Trig0x411400a2/Reco15a/Turbo02/'
                   'Stripping24NoPrescalingFlagged/13164051/ALLSTREAMS.MDST'))

j.inputdata = BK.getDataset()
j.comment = '2015 Bs signal mc, magUp'
j.submit()
