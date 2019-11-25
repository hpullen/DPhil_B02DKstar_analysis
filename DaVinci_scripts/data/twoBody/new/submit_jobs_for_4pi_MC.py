# Script used to submit ganga jobs for data processing
years = [ '12' ]
mags = [ 'Up', 'Down' ]

evt_types = []
evt_types.append('11166055') # 4pi

strippings = {
    '12': 'Nu2.5-Pythia8/Sim09b/Trig0x409f0045/Reco14c/Stripping21NoPrescalingFlagged'
}
files = {
    '12': 'ALLSTREAMS.DST'
}
energies = {
    '12': '4000'
}
conddb = {
    '12': {'Up': 'sim-20160321-2-vc-mu100', 'Down': 'sim-20160321-2-vc-md100'}
}
dddb = {
    '12': 'dddb-20150928'
}

# Shell commands to get conddb tags:
# export PATH=$PATH:/afs/cern.ch/user/i/ibelyaev/public/scripts
# lhcb-proxy-init 
# get_bookkeeping_info  12165102

options_dir = "/home/pullen/analysis/B02DKstar/DaVinci_scripts/data/twoBody/new/"
options = 'B2DKstar_withRestrip_options.py'
DaVinci_dir = '/home/pullen/cmtuser'
DaVinci_version = 'v39r1p1'

for y in years:
    for m in mags:
        for et in evt_types:

            # Create job
            job_name = '{}_{}_{}_{}'.format(et, m, y, "MC")
            print('Preparing job:{}'.format(job_name))
            j = Job(name = job_name,
                    backend = Dirac(),
                    splitter = SplitByFiles(filesPerJob = 20, maxFiles = -1),
                    outputfiles = [
                        DiracFile('*.root'),
                        LocalFile('*.xml')]
                    )
            
            # Set up application
            #  myApp = prepareGaudiExec('DaVinci', DaVinci_version, DaVinci_dir)
            myApp = GaudiExec()
            j.application = myApp
            j.application.options   = ['{}/{}'.format(options_dir, options)]
            j.application.directory = '{}/DaVinciDev_{}'.format(DaVinci_dir, 
                    DaVinci_version)
            j.application.platform = 'x86_64-slc6-gcc49-opt'

            # Set up data location
            bk_path ='/MC/20{Y}/Beam{E}GeV-20{Y}-Mag{M}-{S}/{ET}/{F}' \
                        .format(Y=y, E=energies[y], M=m, S=strippings[y], 
                                ET=et, F=files[y])
            print(bk_path)
            BK = BKQuery(dqflag='OK', path=bk_path)
            j.inputdata = BK.getDataset()

            j.comment = '4pi restripped MC {}_{}'.format(y,m)

            j.application.extraOpts = (
                "import sys, os\n"
                "sys.path.append(os.getcwd())\n"
                "from B2DKstar_withRestrip_options import setup_options\n"
                "setup_options("
                "'{y}'"
                ",'{m}'"
                ",'{f}'"
                ", simulation=True"
                ", conddb='{cdb}'"
                ", dddb='{ddb}'"
                ", event_type='{et}'"
                ")\n").format(y=y, m=m, f=files[y].split(".")[-1], et=et,
                cdb=conddb[y][m], ddb=dddb[y])
            j.submit()

