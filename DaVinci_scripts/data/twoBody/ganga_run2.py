import argparse

def parseArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument('--year', default = None, choices= ['15', '16'])
    parser.add_argument('--mag', default = None, choices= ['Down', 'Up'])
    args = parser.parse_args()
    return args.year, args.mag


# Read inout args
year, mag = parseArgs()

# Check args are valid
if (year == None or mag == None):
    print("Error: invalid arguments. Usage: ganga job_run2.py <15/17> <Up/Down>")

else:
    # Print year and polarity
    print("Using year: 20{0}".format(year))
    print("Using polarity: {0}".format(mag))

    # Stripping and reco for each year
    recoDict = {
            "15" : "15a",
            "16" : "16"
    }
    strippingDict = {
            "15" : "24r1",
            "16" : "28r1"
    }

    # Make job
    jobName = "{0}_data_{1}".format(year, mag)
    j = Job(name = jobName)

    # Make application and options
    myApp = GaudiExec()
    myApp.directory = '/home/pullen/cmtuser/DaVinciDev_v42r6p1'
    myApp.platform = 'x86_64-slc6-gcc62-opt'
    j.application = myApp
    j.application.options = ['run2_options.py']

    # Add year options to DaVinci options
    extraOpts = ("from Configurables import DaVinci\nDaVinci().DataType = 20{0}\n"
                 "CondDB(LatestGlobalTagByDataType = 20{0})").format(year)
    j.application.extraOpts = extraOpts

    # Data location
    #  BK_string = ("//LHCb/Collision{0}/Beam6500GeV-VeloClosed-Mag{1}/Real Data/"
                 #  "Reco{2}/Stripping{3}/90000000/BHADRON.MDST").format(year, mag,
                         #  recoDict[year], strippingDict[year])
    BK_string = ('//MC/2011/Beam3500GeV-2011-MagDown-Nu2-Pythia8/Sim08e/Digi13/"
            "Trig0x40760037/Reco14a/Stripping20r1NoPrescalingFlagged/11164011/"
            "ALLSTREAMS.DST')
    print("Getting data from: {0}".format(BK_string))
    BK = BKQuery(path=BK_string)
    j.inputdata = BK.getDataset()

    # Other job settings
    j.outputfiles=[LocalFile('*.root')]
    j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = -1)
    j.backend = Dirac()
    j.comment = "{0} data {1} {2}".format(year, strippingDict[year], mag)

    # Submit job
    j.submit()
