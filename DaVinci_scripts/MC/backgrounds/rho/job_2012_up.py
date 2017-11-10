j = Job(name='rho0_up',
        backend=Dirac(),
        application=DaVinci(version="v41r3"),
        splitter=SplitByFiles(filesPerJob=10, maxFiles=-1)
        )

j.application.optsfile = '2012_Bd_Drho0.py'

BK = BKQuery(path='//MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim08e/Digi13/Trig0x409f0045/Reco14a/Stripping20NoPrescalingFlagged/11164012/ALLSTREAMS.DST')

j.inputdata = BK.getDataset()
j.comment = '2012 Bd->D0rho0 monte carlo, magup'
j.submit()
