j = Job(name='2016_Bs_g010_up',
        backend=Dirac(),
        application=DaVinci(version="v41r3"),
        splitter=SplitByFiles(filesPerJob=10, maxFiles=-1)
        )

j.application.optsfile = '2016_Bd_DKstar.py'

BK = BKQuery(path=('//MC/2016/Beam6500GeV-2016-MagUp-Nu1.6-25ns-Pythia8/'
                   'Sim09b/Trig0x6138160F/Reco16/Turbo03/'
                   'Stripping26NoPrescalingFlagged/13164245/ALLSTREAMS.MDST'))

j.inputdata = BK.getDataset()
j.comment = '2016 Bs gamma 010, magUp'
j.submit()
