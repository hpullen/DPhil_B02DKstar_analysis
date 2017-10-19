j = Job(name='Bs_sig_2012_up',
        backend=Dirac(),
        application=DaVinci(version = "v41r3"),
        splitter=SplitByFiles(filesPerJob = 10, maxFiles = -1)
        )

j.application.optsfile = '2012_Bd_DKstar.py'

BK = BKQuery(path=('//MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim08b/'
                   'Digi13/Trig0x409f0045/Reco14a/'
                   'Stripping20NoPrescalingFlagged/'
                   '13164051/ALLSTREAMS.DST'))

j.inputdata = BK.getDataset()
j.comment = 'Bs signal magUp 2012'
j.submit()
