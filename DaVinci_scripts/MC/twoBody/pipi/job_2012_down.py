j = Job(name='pipi_2012_down',
        backend=Dirac(),
        application=DaVinci(version = "v41r3"),
        splitter=SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2012_pipi.py'

BK = BKQuery(path=('//MC/2012/Beam4000GeV-2012-MagDown-Nu2.5-Pythia8/Sim08a/'
                   'Digi13/Trig0x409f0045/Reco14a/'
                   'Stripping20NoPrescalingFlagged/'
                   '11164031/ALLSTREAMS.DST'))

j.inputdata = BK.getDataset()
j.comment = '2012 MC pipi down'
j.submit()
