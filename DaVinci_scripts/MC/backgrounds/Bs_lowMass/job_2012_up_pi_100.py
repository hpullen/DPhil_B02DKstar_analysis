j = Job(name='Bs_pb_2012_up_P100',
        backend=Dirac(),
        application=DaVinci(version = "v41r3"),
        splitter=SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2012_Bd_DKstar.py'

BK = BKQuery(path=('//MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim08a/'
                   'Digi13/Trig0x409f0045/Reco14/'
                   'Stripping20NoPrescalingFlagged/'
                   '13164413/ALLSTREAMS.DST'))

j.inputdata = BK.getDataset()
j.comment = 'Bs pb P100 magUp 2012'
j.submit()
