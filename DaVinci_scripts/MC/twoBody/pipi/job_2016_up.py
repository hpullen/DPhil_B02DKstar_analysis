j = Job(name='pipi_2016_up',
        backend=Dirac(),
        application=DaVinci(version = "v41r3"),
        splitter=SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2016_pipi.py'

BK = BKQuery(path=('//MC/2016/Beam6500GeV-2016-MagUp-Nu1.6-25ns-Pythia8/Sim09b/'
                   'Trig0x6138160F/Reco16/Turbo03/Stripping26NoPrescalingFlagged/'
                   '11164031/ALLSTREAMS.MDST'))

j.inputdata = BK.getDataset()
j.comment = '2016 MC pipi up'
j.submit()
