j = Job(name='Bs_pb_2012_down_G001',
        backend=Dirac(),
        application=DaVinci(version = "v41r3"),
        splitter=SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2012_Bd_DKstar.py'

BK = BKQuery(path=('//MC/2012/Beam4000GeV-2012-MagDown-Nu2.5-Pythia8/Sim08a/'
                   'Digi13/Trig0x409f0045/Reco14/'
                   'Stripping20NoPrescalingFlagged/'
                   '13164217/ALLSTREAMS.DST'))

j.inputdata = BK.getDataset()
j.comment = 'Bs pb G001 magDown 2012'
j.submit()
