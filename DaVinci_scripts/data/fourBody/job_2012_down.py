job_name = '2012_data_4body_down'
print 'Preparing job: ', job_name
j = Job(name = job_name,
        backend = Dirac(),
        application = GaudiExec(),
        splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1),
        outputfiles = [
            DiracFile("*.root"),
            LocalFile("*.xml")]
        )

j.application.options = ['2012_four_body.py']
j.application.directory = '/home/pullen/cmtuser/DaVinciDev_v44r6'

BK = BKQuery(path='/LHCb/Collision12/Beam4000GeV-VeloClosed-MagDown/Real Data/Reco14/Stripping21r0p2/90000000/BHADRON.MDST')
j.inputdata = BK.getDataset()

j.comment = '4body_12down'
j.submit()
