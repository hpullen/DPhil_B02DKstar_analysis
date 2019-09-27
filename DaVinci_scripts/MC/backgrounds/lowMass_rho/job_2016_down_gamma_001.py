j = Job(name='2016_D*rho_g001_down',
        backend=Dirac(),
        application=GaudiExec(),
        splitter=SplitByFiles(filesPerJob=10, maxFiles=-1),
        outputfiles = [
            DiracFile('*.root'),
            LocalFile('*.xml')]
        )

j.application.directory = '/home/pullen/cmtuser/DaVinciDev_v44r6'
j.application.options = ['2016_Bd_DKstar_down.py']

BK = BKQuery(path=('//MC/2016/Beam6500GeV-2016-MagDown-Nu1.6-25ns-Pythia8/'
                   'Sim09c/Trig0x6138160F/Reco16/Turbo03/'
                   'Stripping28r1NoPrescalingFlagged/11164201/ALLSTREAMS.MDST'))

j.inputdata = BK.getDataset()
j.comment = '2016 D*rho gamma 001, magDown'
j.submit()
