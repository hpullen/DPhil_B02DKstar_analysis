from GaudiConf import IOHelper
from Configurables import DaVinci
from Configurables import EventTuple
from Configurables import DecayTreeTuple
from Configurables import L0TriggerTisTos, TriggerTisTos
from Configurables import TupleToolPropertime, TupleToolTISTOS
from Configurables import TupleToolStripping, TupleToolDecayTreeFitter
from Configurables import TupleToolGeometry, TupleToolKinematic
from Configurables import TupleToolPrimaries
from Configurables import TupleToolTrackInfo, TupleToolDecay
from Configurables import TupleToolTrackIsolation
from Configurables import LoKi__Hybrid__TupleTool, LoKi__Hybrid__EvtTupleTool
from DecayTreeTuple.Configuration import *

# ========================================
# Stream and stripping line we want to use
# ========================================
stream = 'AllStreams'
line = 'B02D0KPiD2HHBeauty2CharmLine'

# =============================================================
# Create an ntuple to capture B0 decays from the stripping line
# =============================================================
dtt = DecayTreeTuple('Tuple_Kpi')
dtt.Inputs = ['/Phys/{0}/Particles'.format(line)]
dtt.Decay = ('[[B0 -> ^(D0 -> ^K- ^pi+) ^(K*(892)0 -> ^K+ ^pi-)]CC,'
             '[B0 -> ^(D0 -> ^K+ ^pi-) ^(K*(892)0 -> ^K+ ^pi-)]CC,'
             '[B0 -> ^(D0 -> ^K- ^pi+) ^(K*(892)~0 -> ^K- ^pi+)]CC,'
             '[B0 -> ^(D0 -> ^K+ ^pi-) ^(K*(892)~0 -> ^K- ^pi+)]CC]')

# ============
# Add branches
# ============
dtt.addBranches({
    "Bd": ('[[B0 -> (D0 -> K- pi+) (K*(892)0 -> K+ pi-)]CC,'
           '[B0 -> (D0 -> K+ pi-) (K*(892)0 -> K+ pi-)]CC,'
           '[B0 -> (D0 -> K- pi+) (K*(892)~0 -> K- pi+)]CC,'
           '[B0 -> (D0 -> K+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0": ('[[B0 -> ^(D0 -> K- pi+) (K*(892)0 -> K+ pi-)]CC,'
           '[B0 -> ^(D0 -> K+ pi-) (K*(892)0 -> K+ pi-)]CC,'
           '[B0 -> ^(D0 -> K- pi+) (K*(892)~0 -> K- pi+)]CC,'
           '[B0 -> ^(D0 -> K+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0K": ('[[B0 -> (D0 -> ^K- pi+) (K*(892)0 -> K+ pi-)]CC,'
            '[B0 -> (D0 -> ^K+ pi-) (K*(892)0 -> K+ pi-)]CC,'
            '[B0 -> (D0 -> ^K- pi+) (K*(892)~0 -> K- pi+)]CC,'
            '[B0 -> (D0 -> ^K+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0Pi": ('[[B0 -> (D0 -> K- ^pi+) (K*(892)0 -> K+ pi-)]CC,'
             '[B0 -> (D0 -> K+ ^pi-) (K*(892)0 -> K+ pi-)]CC,'
             '[B0 -> (D0 -> K- ^pi+) (K*(892)~0 -> K- pi+)]CC,'
             '[B0 -> (D0 -> K+ ^pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "Kstar": ('[[B0 -> (D0 -> K- pi+) ^(K*(892)0 -> K+ pi-)]CC,'
              '[B0 -> (D0 -> K+ pi-) ^(K*(892)0 -> K+ pi-)]CC,'
              '[B0 -> (D0 -> K- pi+) ^(K*(892)~0 -> K- pi+)]CC,'
              '[B0 -> (D0 -> K+ pi-) ^(K*(892)~0 -> K- pi+)]CC]'),
    "KstarK": ('[[B0 -> (D0 -> K- pi+) (K*(892)0 -> ^K+ pi-)]CC,'
               '[B0 -> (D0 -> K+ pi-) (K*(892)0 -> ^K+ pi-)]CC,'
               '[B0 -> (D0 -> K- pi+) (K*(892)~0 -> ^K- pi+)]CC,'
               '[B0 -> (D0 -> K+ pi-) (K*(892)~0 -> ^K- pi+)]CC]'),
    "KstarPi": ('[[B0 -> (D0 -> K- pi+) (K*(892)0 -> K+ ^pi-)]CC,'
                '[B0 -> (D0 -> K+ pi-) (K*(892)0 -> K+ ^pi-)]CC,'
                '[B0 -> (D0 -> K- pi+) (K*(892)~0 -> K- ^pi+)]CC,'
                '[B0 -> (D0 -> K+ pi-) (K*(892)~0 -> K- ^pi+)]CC]')
    })

# ===============================
# Group charged daughter branches
# ===============================
Kpi_h = [dtt.D0K, dtt.D0Pi, dtt.KstarK, dtt.KstarPi]

# =====================
# List of trigger lines
# =====================
triggerListL0 = ["L0HadronDecision"]

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
dtt.ToolList = ['TupleToolAngles',
		'TupleToolEventInfo',
		'TupleToolGeometry',
		'TupleToolKinematic',
		'TupleToolPrimaries',
		'TupleToolPropertime',
		'TupleToolRecoStats',
		'TupleToolTrackInfo',
		'TupleToolMCTruth',
		'TupleToolMCBackgroundInfo']

# TupleToolTISTOS
tttistos = dtt.addTupleTool("TupleToolTISTOS/tttistos")
tttistos.VerboseL0 = True
tttistos.VerboseHlt1 = True
tttistos.VerboseHlt2 = True
tttistos.TriggerList = triggerListAll

# TupleToolTrigger
tttrigger = dtt.addTupleTool("TupleToolTrigger/tttrigger")
tttrigger.Verbose = True
tttrigger.TriggerList = triggerListAll

# TupleToolStripping
ttstripping = dtt.addTupleTool("TupleToolStripping/ttstripping")
ttstripping.StrippingList = [("StrippingB02D0KPiD2HH"
                              "Beauty2CharmLineDecision")]

# =======================
# DecayTreeFitter
# =======================
dtt.Bd.addTupleTool('TupleToolDecayTreeFitter/ConsD')
dtt.Bd.ConsD.constrainToOriginVertex = True
dtt.Bd.ConsD.Verbose = True
dtt.Bd.ConsD.daughtersToConstrain = ['D0']
dtt.Bd.ConsD.UpdateDaughters = True

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
LoKi_Bd_extra_vars = {
        "LV01":        "LV01",
        "LV02":        "LV02",
        "ptasy_1.50": ("RELINFO('/Event/AllStreams/Phys/"
                       "B02D0KPiD2HHBeauty2CharmLine/P2ConeVar1',"
                       "'CONEPTASYM',-1000.)"),
        "ptasy_1.70": ("RELINFO('/Event/AllStreams/Phys/"
                       "B02D0KPiD2HHBeauty2CharmLine/P2ConeVar2',"
                       "'CONEPTASYM',-1000.)")
        }
LoKi_Bd_vars = dict(LoKi_Kstar_vars.items() + LoKi_Bd_extra_vars.items())

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
for branch in Kpi_h:
    LoKi_h = branch.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_h")
    LoKi_h.Preambulo = ["from LoKiTracks.decorators import *"]
    LoKi_h.Variables = LoKi_h_vars

LoKi_Bd = dtt.Bd.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_Bd")
LoKi_Bd.Variables = LoKi_Bd_vars

LoKi_Kstar = dtt.Kstar.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_Kstar")
LoKi_Kstar.Variables = LoKi_Kstar_vars

LoKi_D0 = dtt.D0.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_D0")
LoKi_D0.Variables = LoKi_D0_vars

LoKi_evt = dtt.addTupleTool("LoKi::Hybrid::EvtTupleTool/LoKi_evt")
LoKi_evt.Preambulo = ['from LoKiTracks.decorators import *',
                      'from LoKiNumbers.decorators import *',
                      'from LoKiCore.functions import *']
LoKi_evt.VOID_Variables = LoKi_evt_vars


# =================
# Configure DaVinci
# =================
DaVinci().UserAlgorithms += [dtt]
DaVinci().RootInTES = '/Event/{0}'.format(stream)
DaVinci().InputType = 'MDST'
DaVinci().TupleFile = 'Tuple_Bd_DKstar.root'
DaVinci().PrintFreq = 1000
DaVinci().DataType = '2016'
DaVinci().Simulation = True
DaVinci().Lumi = False
DaVinci().EvtMax = -1  # Process all events
DaVinci().DDDBtag = 'dddb-20170721-3'
DaVinci().CondDBtag = 'sim-20170721-2-vc-mu100'
