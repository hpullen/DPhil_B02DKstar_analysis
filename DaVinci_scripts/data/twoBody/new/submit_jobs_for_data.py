# Script used to submit ganga jobs for data processing

#  years = [ '17', '18' ]
#  mags  = [ 'Down', 'Up']
years = [ '17', '18' ]
mags  = [ 'Down', 'Up' ]

strippings = {
    '11': 'Reco14/Stripping21r1', 
    '12': 'Reco14/Stripping21', 
    '15': 'Reco15a/Stripping24r1', 
    '16': 'Reco16/Stripping28r1', 
    '17': 'Reco17/Stripping29r2',
    '18': 'Reco18/Stripping34'}

files = {
    '11': 'BHADRON.MDST', 
    '12': 'BHADRON.MDST', 
    '15': 'BHADRON.MDST', 
    '16': 'BHADRON.MDST', 
    '17': 'BHADRON.MDST',
    '18': 'BHADRON.MDST'}

energies = {
    '11': '3500', 
    '12': '4000', 
    '15': '6500', 
    '16': '6500', 
    '17': '6500',
    '18': '6500'}

options_dir = "/home/pullen/analysis/B02DKstar/DaVinci_scripts/data/twoBody/new/"
options_2body = 'B2DKstar_davinci_options.py'
options_4body = 'B2DKstar_4body_davinci_options.py'
DaVinci_dir = '/home/pullen/cmtuser'
DaVinci_version = 'v44r6'


for y in years:
    for m in mags:

        # General setup
        bk_path ='/LHCb/Collision{Y}/Beam{E}GeV-VeloClosed-Mag{M}/Real Data/{S}/90000000/{F}' \
                    .format(Y=y, E=energies[y], M=m, S=strippings[y], F=files[y])
        BK = BKQuery(path=bk_path)

        job_name = 'B2DKstar_{}_{}_{}'.format(m, y, "data")
        print 'Preparing job:', job_name
        j = Job(name = job_name,
                backend = Dirac(),
                application = GaudiExec(),
                splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1),
                outputfiles = [
                    DiracFile('*.root'),
                    LocalFile('*.xml')]
                )

        j.application.options   = ['{}/{}'.format(options_dir, options_2body)]
        j.application.directory = '{}/DaVinciDev_{}'.format(DaVinci_dir, DaVinci_version)

        j.inputdata = BK.getDataset()

        j.comment = 'data: {}'.format(job_name)

        j.application.extraOpts = (
            "import sys, os\n"
            "sys.path.append(os.getcwd())\n"
            "from B2DKstar_davinci_options import setup_options\n"
            "setup_options('{}','{}','{}')\n").format(
                y, m, files[y].split(".")[-1])
        j.submit()

        #  4-body job
        #  job_name4 = 'B2DKstar_4body_{}_{}_{}'.format(m, y, "data")
        #  print 'Preparing job:', job_name4
        #  j4 = Job(name = job_name4,
                #  backend = Dirac(),
                #  application = GaudiExec(),
                #  splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1),
                #  outputfiles = [
                    #  DiracFile('*.root'),
                    #  LocalFile('*.xml')]
                #  )

        #  j4.application.options   = ['{}/{}'.format(options_dir, options_4body)]
        #  j4.application.directory = '{}/DaVinciDev_{}'.format(DaVinci_dir, DaVinci_version)
        #  j4.inputdata = BK.getDataset()

        #  j4.comment = 'data: 4-body {}'.format(job_name4)

        #  j4.application.extraOpts = (
            #  "import sys, os\n"
            #  "sys.path.append(os.getcwd())\n"
            #  "from B2DKstar_4body_davinci_options import setup_options\n"
            #  "setup_options('{}','{}','{}')\n").format(
                #  y, m, files[y].split(".")[-1])
        #  j4.submit()

