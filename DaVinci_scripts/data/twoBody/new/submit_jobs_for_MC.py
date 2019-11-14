# Script used to submit ganga jobs for data processing

years = [ '17', '18' ]
mags = [ 'Up', 'Down' ]

evt_types = []
evt_types.append('11164011') # Kpi
evt_types.append('11164021') # KK
evt_types.append('11164031') # pipi
evt_types.append('11166053') # Kpipipi
evt_types.append('11166055') # pipipipi

strippings = {
    '17': 'Nu1.6-25ns-Pythia8/Sim09h-ReDecay01/Trig0x62661709/Reco17/Turbo04a-WithTurcal/Stripping29r2NoPrescalingFlagged',
    '18': 'Nu1.6-25ns-Pythia8/Sim09h-ReDecay01/Trig0x617d18a4/Reco18/Turbo05-WithTurcal/Stripping34NoPrescalingFlagged'
}
files = {
    '17': 'ALLSTREAMS.MDST', 
    '18': 'ALLSTREAMS.MDST'
}
energies = {
    '17': '6500',
    '18': '6500'
}
conddb = {
    '17': {'Up': 'sim-20190430-1-vc-mu100', 'Down': 'sim-20190430-1-vc-md100'},
    '18': {'Up': 'sim-20190430-vc-mu100', 'Down': 'sim-20190430-vc-md100'},
}
dddb = {
    '17': 'dddb-20170721-3',
    '18': 'dddb-20170721-3'
}

# Shell commands to get conddb tags:
# export PATH=$PATH:/afs/cern.ch/user/i/ibelyaev/public/scripts
# lhcb-proxy-init 
# get_bookkeeping_info  12165102

options_dir = "/home/pullen/analysis/B02DKstar/DaVinci_scripts/data/twoBody/new/"
options = 'B2DKstar_davinci_options.py'
DaVinci_dir = '/home/pullen/cmtuser'
DaVinci_version = 'v44r6'

for y in years:
    for m in mags:
        for et in evt_types:

            # Ignore missing file!
            if y == '18' and m == 'Down' and et == '11164021':
                continue

            job_name = '{}_{}_{}_{}'.format(et, m, y, "MC")
            print('Preparing job:{}'.format(job_name))
            j = Job(name = job_name,
                    backend = Dirac(),
                    application = GaudiExec(), # automatically find correct DB tags
                    splitter = SplitByFiles(filesPerJob = 20, maxFiles = -1),
                    outputfiles = [
                        DiracFile('*.root'),
                        LocalFile('*.xml')]
                    )

            j.application.options   = ['{}/{}'.format(options_dir, options)]
            j.application.directory = '{}/DaVinciDev_{}'.format(DaVinci_dir, DaVinci_version)
            bk_path ='/MC/20{Y}/Beam{E}GeV-20{Y}-Mag{M}-{S}/{ET}/{F}' \
                        .format(Y=y, E=energies[y], M=m, S=strippings[y], ET=et, F=files[y])
            print(bk_path)
            BK = BKQuery(dqflag='OK', path=bk_path)
            j.inputdata = BK.getDataset()

            j.comment = 'MC w/BKGCAT: {}'.format(job_name)

            j.application.extraOpts = (
                "import sys, os\n"
                "sys.path.append(os.getcwd())\n"
                "from B2DKstar_davinci_options import setup_options\n"
                "setup_options("
                "'{y}'"
                ",'{m}'"
                ",'{f}'"
                ", simulation=True"
                ", conddb='{cdb}'"
                ", dddb='{ddb}'"
                ", event_type='{et}'"
                ", stream = 'AllStreams'"
                ")\n").format(y=y, m=m, f=files[y].split(".")[-1], et=et,
                cdb=conddb[y][m], ddb=dddb[y])
            j.submit()

