from GaudiConf import IOHelper
from Configurables import DaVinci
from Configurables import EventTuple
from Configurables import DecayTreeTuple
from Configurables import L0TriggerTisTos, TriggerTisTos
from Configurables import TupleToolPropertime, TupleToolTISTOS
from Configurables import TupleToolStripping, TupleToolDecayTreeFitter
from Configurables import TupleToolGeometry, TupleToolKinematic
from Configurables import TupleToolPid, TupleToolPrimaries
from Configurables import TupleToolTrackInfo, TupleToolDecay
from Configurables import TupleToolTrackIsolation
from Configurables import CondDB
from Configurables import LoKi__Hybrid__TupleTool, LoKi__Hybrid__EvtTupleTool
from DecayTreeTuple.Configuration import *

from StrippingConf.Configuration import StrippingConf, StrippingStream
from StrippingSettings.Utils import strippingConfiguration
from StrippingArchive.Utils import buildStreams
from StrippingArchive import strippingArchive
from Configurables import EventNodeKiller

def setup_options(
    year,
    mag,
    input_type, #DST or MDST
    test = False, # are we just running on test data?
    test_files = None, # files for IOHelper if running test
    simulation = False, # data or mc
    event_type = None, # the 8 digit event code for sim, used for ntuple naming
    conddb = None, # used for MC
    dddb = None, # used for MC
    event_max = -1,
    test_output = None,
    ):

    #==================================#
    #=== Stream and stripping lines ===#
    #==================================#
    eventNodeKiller = EventNodeKiller('Stripkiller')
    eventNodeKiller.Nodes = [ '/Event/AllStreams', '/Event/Strip' ]
    stream = 'MyStream'
    line = 'B02D0KPiD2HHHHBeauty2CharmLine'
    strippingList = ["Stripping{0}Decision".format(line)]

    # 2012 restripping
    stripping = 'stripping21r0p2'
    config  = strippingConfiguration(stripping)
    archive = strippingArchive(stripping)
    streams = buildStreams(stripping=config, archive=archive)

    # Select my line
    MyStream = StrippingStream(stream)
    MyLines = ['Stripping{0}'.format(line)]
    for stream in streams:
        for line in stream.lines:
            if line.name() in MyLines:
                MyStream.appendLines( [ line ] )

    # Configure Stripping
    from Configurables import ProcStatusCheck
    filterBadEvents = ProcStatusCheck()

    sc = StrippingConf( Streams = [ MyStream ],
                        MaxCandidates = 2000,
                        AcceptBadEvents = False,
                        BadEventSelection = filterBadEvents )

    #=========================#
    #=== Configure DaVinci ===#
    #=========================#

    DaVinci().EvtMax = event_max
    DaVinci().Simulation = simulation
    DaVinci().InputType = input_type
    DaVinci().DataType = "20{}".format(year)
    DaVinci().TupleFile = 'Tuple_Bd_DKstar.root'

    if simulation:
        # make sure tags and stream are there for MC
        if None in (stream, conddb, dddb, event_type):
            print "b2dh_davinci_options.py :: For simulation you MUST specify:"
            print "b2dh_davinci_options.py ::   conddb (was {})".format(conddb)
            print "b2dh_davinci_options.py ::   dddb (was {})".format(dddb)
            print "b2dh_davinci_options.py ::   stream (was {})".format(stream)
            sys.exit(-1)
        DaVinci().CondDBtag = conddb
        DaVinci().DDDBtag = dddb
        DaVinci().TupleFile = "Bu2Dh_D2Kshh_{}_{}_{}.root".format(event_type, year, mag)

    else:
        # for data, make sure the correct stream is used
        DaVinci().Lumi = True
        if input_type=="MDST":
            stream = "Bhadron"
        else:
            stream = "BhadronCompleteEvent"

    # set input TES and location predix based in inputs
    if input_type=="MDST":
        DaVinci().RootInTES = '/Event/%s' % stream
        location_prefix = ""
    else:
        location_prefix = "/Event/{}".format(stream)

    if test: # run locally on specified test files
        if test_output == None:
            print "B2DKstar_davinci_options.py :: please provide output file when running test!"
            sys.exit(-1)
        DaVinci().TupleFile = test_output
        IOHelper().inputFiles(test_files, clear=True)

    # =================
    # Decay descriptors
    # =================
    dec_pipipipi = ('[[B0 -> ^(D0 -> ^pi+ ^pi- ^pi+ ^pi-) ^(K*(892)0 -> ^K+ ^pi-)]CC,'
                '[B0 -> ^(D0 -> ^pi+ ^pi- ^pi+ ^pi-) ^(K*(892)~0 -> ^K- ^pi+)]CC]')

    # ======================
    # Set up DecayTreeTuples
    # ======================
    tuple_pipipipi = DecayTreeTuple('Tuple_pipipipi')

    tupleList_4body = [tuple_pipipipi]
    tupleList = tupleList_4body

    for ntp in tupleList:
        ntp.Inputs = ['/Phys/{0}/Particles'.format(line)]

    # ==================
    # Assign decay modes
    # ==================
    tuple_pipipipi.Decay = dec_pipipipi

    # ============
    # Add Branches
    # ============
    tuple_pipipipi.addBranches({
        "Bd":        ('[[B0 -> (D0 -> pi- pi+ pi+ pi-) (K*(892)0 -> K+ pi-)]CC,'
                      '[B0 -> (D0 -> pi+ pi- pi+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
        "D0":        ('[[B0 -> ^(D0 -> pi- pi+ pi+ pi-) (K*(892)0 -> K+ pi-)]CC,'
                      '[B0 -> ^(D0 -> pi+ pi- pi+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
        "D0PiPlus1":  ('[[B0 -> (D0 -> ^pi+ pi- pi+ pi-) (K*(892)0 -> K+ pi-)]CC,'
                      '[B0 -> (D0 -> ^pi+ pi- pi+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
        "D0PiMinus1": ('[[B0 -> (D0 -> pi+ ^pi- pi+ pi-) (K*(892)0 -> K+ pi-)]CC,'
                      '[B0 -> (D0 -> pi+ ^pi- pi+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
        "D0PiPlus2":  ('[[B0 -> (D0 -> pi+ pi- ^pi+ pi-) (K*(892)0 -> K+ pi-)]CC,'
                      '[B0 -> (D0 -> pi+ pi- ^pi+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
        "D0PiMinus2": ('[[B0 -> (D0 -> pi+ pi- pi+ ^pi-) (K*(892)0 -> K+ pi-)]CC,'
                      '[B0 -> (D0 -> pi+ pi- pi+ ^pi-) (K*(892)~0 -> K- pi+)]CC]'),
        "Kstar":     ('[[B0 -> (D0 -> pi- pi+ pi+ pi-) ^(K*(892)0 -> K+ pi-)]CC,'
                      '[B0 -> (D0 -> pi+ pi- pi+ pi-) ^(K*(892)~0 -> K- pi+)]CC]'),
        "KstarK":    ('[[B0 -> (D0 -> pi- pi+ pi+ pi-) (K*(892)0 -> ^K+ pi-)]CC,'
                      '[B0 -> (D0 -> pi+ pi- pi+ pi-) (K*(892)~0 -> ^K- pi+)]CC]'),
        "KstarPi":   ('[[B0 -> (D0 -> pi- pi+ pi+ pi-) (K*(892)0 -> K+ ^pi-)]CC,'
                      '[B0 -> (D0 -> pi+ pi- pi+ pi-) (K*(892)~0 -> K- ^pi+)]CC]'),
        })

    # ===============================
    # Group charged daughter branches
    # ===============================
    pipipipi_h = [tuple_pipipipi.D0PiPlus1, tuple_pipipipi.D0PiMinus1, 
                  tuple_pipipipi.D0PiPlus2, tuple_pipipipi.D0PiMinus2, 
                  tuple_pipipipi.KstarK, tuple_pipipipi.KstarPi]
    branchList_h = pipipipi_h

    # =====================
    # List of trigger lines
    # =====================
    triggerListL0 = ["L0HadronDecision"]

    triggerListHlt1 = []
    triggerListHlt2 = []

    if year == "11" or year == "12":
        triggerListHlt1 = ["Hlt1TrackAllL0Decision"]

        triggerListHlt2 = ["Hlt2Topo2BodyBBDTDecision",
                           "Hlt2Topo3BodyBBDTDecision",
                           "Hlt2Topo4BodyBBDTDecision"]
    else: 
        triggerListHlt1 = ["Hlt1TrackMVADecision",
                           "Hlt1TwoTrackMVADecision",
                           "Hlt1TrackPhotonDecision",
                           "Hlt1TrackForwardPassThroughDecision",
                           "Hlt1TrackForwardPassThroughLooseDecision"]

        triggerListHlt2 = ["Hlt2Topo2BodyDecision",
                           "Hlt2Topo3BodyDecision",
                           "Hlt2Topo4BodyDecision"]

    triggerListAll = triggerListL0 + triggerListHlt1 + triggerListHlt2

    # ==============
    # Add TupleTools
    # ==============
    for ntp in tupleList:
        if simulation:
            ntp.ToolList = ['TupleToolAngles',
                            'TupleToolEventInfo',
                            'TupleToolGeometry',
                            'TupleToolKinematic',
                            'TupleToolPid',
                            'TupleToolPrimaries',
                            'TupleToolPropertime',
                            'TupleToolRecoStats',
                            'TupleToolTrackInfo',
                            'TupleToolMCTruth',
                            'TupleToolMCBackgroundInfo']
        else:
            ntp.ToolList = ['TupleToolAngles',
                            'TupleToolEventInfo',
                            'TupleToolGeometry',
                            'TupleToolKinematic',
                            'TupleToolPid',
                            'TupleToolPrimaries',
                            'TupleToolPropertime',
                            'TupleToolRecoStats',
                            'TupleToolTrackInfo']

        # TupleToolTISTOS
        tttistos = ntp.addTupleTool("TupleToolTISTOS/tttistos")
        tttistos.VerboseL0 = True
        tttistos.VerboseHlt1 = True
        tttistos.VerboseHlt2 = True
        tttistos.TriggerList = triggerListAll

        # TupleToolTrigger
        tttrigger = ntp.addTupleTool("TupleToolTrigger/tttrigger")
        tttrigger.Verbose = True
        tttrigger.TriggerList = triggerListAll

        # Stripping
        tts = ntp.addTupleTool("TupleToolStripping/tts")
        tts.StrippingList = strippingList


    # =========================
    # Add DecayTreeFitter to B0
    # =========================
    for ntp in tupleList:
        ntp.Bd.addTupleTool('TupleToolDecayTreeFitter/ConsD')
        ntp.Bd.ConsD.constrainToOriginVertex = True
        ntp.Bd.ConsD.Verbose = True
        ntp.Bd.ConsD.daughtersToConstrain = ['D0']
        ntp.Bd.ConsD.UpdateDaughters = True

    # ==============
    # LoKi variables
    # ==============
    # Charged daughter LoKi variables
    LoKi_h_vars = {
            "Q": "Q",
            "MIP_PV":      "MIPDV(PRIMARY)",
            "MIPCHI2_PV":  "MIPCHI2DV(PRIMARY)",
            "TRCHI2DOF":   "TRCHI2DOF",
            "TRGHOSTPROB": "TRGHP",
            "TRTYPE":      "TRTYPE",
            "ETA":         "ETA",
            "Y":           "Y"
            }

    # K*0 LoKi variables
    LoKi_Kstar_vars = {
            "Q":                     "Q",
            "DIRA_BPV":              "BPVDIRA",
            "MAXDOCA":               "DOCAMAX",
            "AMAXDOCA":              "PFUNA(AMAXDOCA(''))",
            "AMINDOCA":              "PFUNA(AMINDOCA(''))",
            "MIP_PV":                "MIPDV(PRIMARY)",
            "MIPCHI2_PV":            "MIPCHI2DV(PRIMARY)",
            "IP_BPV":                "BPVIP()",
            "IPCHI2_BPV":            "BPVIPCHI2()",
            "FD_BPV":                "BPVVD",
            "FD_BPV_SIGNED":         "BPVVDSIGN",
            "FDCHI2_BPV":            "BPVVDCHI2",
            "RHO_BPV":               "BPVVDRHO",
            "Z_BPV":                 "BPVVDZ",
            "LOKI_VFASPF_VCHI2VDOF": "VFASPF(VCHI2/VDOF)",
            "ENDVTX_X":              "VFASPF(VX)",
            "ENDVTX_Y":              "VFASPF(VY)",
            "ENDVTX_Z":              "VFASPF(VZ)",
            "ETA":                   "ETA",
            "Y":                     "Y",
            "LT_BPV":                ("BPVLTIME('PropertimeFitter/ProperTime"
                                      "::PUBLIC')"),
            "LTCHI2_BPV":            ("BPVLTCHI2('PropertimeFitter/ProperTime"
                                      "::PUBLIC')"),
            "LTFITCHI2_BPV":         ("BPVLTFITCHI2('PropertimeFitter/ProperTime"
                                      "::PUBLIC')")
            }

    # D0 LoKi variables
    LoKi_D0_vars = LoKi_Kstar_vars

    # B LoKi variables
    LoKi_Bd_extra_vars_2body = {
            "LV01":        "LV01",
            "LV02":        "LV02",
            "ptasy_1.50": ("RELINFO('/Event/Bhadron/Phys/"
                           "B02D0KPiD2HHBeauty2CharmLine/P2ConeVar1',"
                           "'CONEPTASYM', -1000.)"),
            "ptasy_1.70": ("RELINFO('/Event/Bhadron/Phys/"
                           "B02D0KPiD2HHBeauty2CharmLine/P2ConeVar2',"
                           "'CONEPTASYM', -1000.)")
            }
    LoKi_Bd_extra_vars_4body = {
            "LV01":        "LV01",
            "LV02":        "LV02",
            "ptasy_1.50": ("RELINFO('/Event/Bhadron/Phys/"
                           "B02D0KPiD2HHHHBeauty2CharmLine/P2ConeVar1',"
                           "'CONEPTASYM', -1000.)"),
            "ptasy_1.70": ("RELINFO('/Event/Bhadron/Phys/"
                           "B02D0KPiD2HHHHBeauty2CharmLine/P2ConeVar2',"
                           "'CONEPTASYM', -1000.)")
            }
    LoKi_Bd_vars_2body = dict(LoKi_Kstar_vars.items() + LoKi_Bd_extra_vars_2body.items())
    LoKi_Bd_vars_4body = dict(LoKi_Kstar_vars.items() + LoKi_Bd_extra_vars_4body.items())

    # LoKi variables for event
    LoKi_evt_vars = {
            # Track information
            "LOKI_nTracks": "RECSUMMARY(LHCb.RecSummary.nTracks, -1)",
            "LOKI_nLong": "RECSUMMARY(LHCb.RecSummary.nLongTracks, -1)",
            # RICH multiplicities
            "LOKI_nRICH1Hits": "switch(HASRECSUMMARY(20), RECSUMMARY(20, -1), -1)",
            "LOKI_nRICH2Hist": "switch(HASRECSUMMARY(21), RECSUMMARY(21, -1), -1)"
            }

    # ==================
    # Add LoKi variables
    # ==================
    for branch in branchList_h:
        LoKi_h = branch.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_h")
        LoKi_h.Preambulo = ["from LoKiTracks.decorators import *"]
        LoKi_h.Variables = LoKi_h_vars

    for ntp in tupleList_2body:
        LoKi_Bd = ntp.Bd.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_Bd")
        LoKi_Bd.Variables = LoKi_Bd_vars_2body

    for ntp in tupleList_4body:
        LoKi_Bd = ntp.Bd.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_Bd")
        LoKi_Bd.Variables = LoKi_Bd_vars_4body

    for ntp in tupleList:
        LoKi_Kstar = ntp.Kstar.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_Kstar")
        LoKi_Kstar.Variables = LoKi_Kstar_vars

        LoKi_D0 = ntp.D0.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_D0")
        LoKi_D0.Variables = LoKi_D0_vars

        LoKi_evt = ntp.addTupleTool("LoKi::Hybrid::EvtTupleTool/LoKi_evt")
        LoKi_evt.Preambulo = ['from LoKiTracks.decorators import *',
                              'from LoKiNumbers.decorators import *',
                              'from LoKiCore.functions import *']
        LoKi_evt.VOID_Variables = LoKi_evt_vars

    # ======
    # Scaler
    # ======
    if not simulation:
        from Configurables import TrackScaleState
        scaler = TrackScaleState('StateScale')
        if input_type == "MDST":
            scaler.RootInTES = '/Event/{0}/'.format(stream) 
        DaVinci().appendToMainSequence([scaler])
        CondDB(LatestGlobalTagByDataType = "20{}".format(year))

    DaVinci().RootInTES = '/Event/{0}'.format(stream)

    #========================================#
    #=== Add tuples to list of algorithms ===#
    #========================================#
    DaVinci().UserAlgorithms += tupleList
