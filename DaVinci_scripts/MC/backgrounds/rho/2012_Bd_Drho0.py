from GaudiConf import IOHelper
# from Gaudi.Configuration import FileCatalog
from Configurables import L0TriggerTisTos, TriggerTisTos
from Configurables import DaVinci, DecayTreeTuple
from Configurables import TupleToolStripping, TupleToolDecayTreeFitter
from Configurables import  TupleToolGeometry, TupleToolKinematic, TupleToolPid, TupleToolPrimaries, TupleToolTrackInfo, LoKi__Hybrid__TupleTool, LoKi__Hybrid__EvtTupleTool, TupleToolTrackIsolation
from DecayTreeTuple.Configuration import *

# Runs on 2012 monte carlo for Kpi mode

# ========================================
# Stream and stripping line we want to use
# ========================================
stream = 'AllStreams'
line = 'B02D0KPiD2HHBeauty2CharmLine'

# =============================================================
# Create an ntuple to capture B0 decays from the stripping line
# =============================================================
dtt = DecayTreeTuple('Tuple_Kpi')
dtt.Inputs = ['/Event/{0}/Phys/{1}/Particles'.format(stream, line)]
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

# =====================
# List of trigger lines
# =====================
triggerListL0 = ["L0HadronDecision"]

triggerListHlt1 = ["Hlt1TrackAllL0Decision"]

triggerListHlt2 = ["Hlt2Topo2BodyBBDTDecision",
                   "Hlt2Topo3BodyBBDTDecision",
                   "Hlt2Topo4BodyBBDTDecision"]

triggerListAll = triggerListL0 + triggerListHlt1 + triggerListHlt2

# ==============
# Add TupleTools
# ==============
dtt.ToolList = ['TupleToolEventInfo',
                'TupleToolAngles',
                'TupleToolGeometry',
                'TupleToolKinematic',
                'TupleToolPid',
                'TupleToolPrimaries',
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


# =============
# LoKi functors
# =============
def ParentConfig(branch):
    LokiToolParent = LoKi__Hybrid__TupleTool('LokiToolParent')
    LokiToolParent.Variables = {
        "LOKI_VFASPF_VCHI2": "VFASPF(VCHI2)",
        "LOKI_VFASPF_VDOF": "VFASPF(VDOF)",
        "LOKI_VFASPF_VCHI2VDOF": "VFASPF(VCHI2/VDOF)",
        "LOKI_VMINVDDV": "VFASPF(VMINVDDV(PRIMARY))",
        "LOKI_VMINVDDV": "VFASPF(VMINVDDV(PRIMARY))",
        "LOKI_MM": "MM",
        "LOKI_M": "M",
        "LOKI_VMINVDDV": "VFASPF(VMINVDDV(PRIMARY))",
        "LOKI_BPVLTCHI2": "BPVLTCHI2()",
        "LOKI_BPVLTIME": "BPVLTIME()",
        "LOKI_BPVVDR": "BPVVDR",
        "LOKI_BPVVDZ": "BPVVDZ",
        "LOKI_BPVVDZ": "BPVVDZ",
        "LOKI_BPVDIRA":  "BPVDIRA",
        "LOKI_DOCAMAX": "DOCAMAX",
        "LOKI_DOCACHI2MAX": "DOCACHI2MAX",
        "LOKI_CORRM": "BPVCORRM",
        "LOKI_AMAXDOCA":
            "LoKi.Particles.PFunA(AMAXDOCA('LoKi::TrgDistanceCalculator'))",
        "LOKI_AMINDOCA":
            "LoKi.Particles.PFunA(AMINDOCA('LoKi::TrgDistanceCalculator'))"
        }
    branch.addTool(LokiToolParent)
    branch.ToolList += ['LoKi::Hybrid::TupleTool/LokiToolParent']


def CommonConfig(branch):
    LokiToolCommon = LoKi__Hybrid__TupleTool('LokiToolCommon')
    LokiToolCommon.Variables = {
        "LOKI_ETA": "ETA",
        "LOKI_Y": "Y",
        "LOKI_ABSID": "ABSID",
        "LOKI_BPVIPCHI2": "BPVIPCHI2()",
        "LOKI_ID": "ID",
        "LOKI_MIPDV_PRIMARY": "MIPDV(PRIMARY)",
        "LOKI_MIPCHI2DV_PRIMARY": "MIPCHI2DV(PRIMARY)",
        "LOKI_P": "P",
        "LOKI_PT": "PT",
        "LOKI_TRCHI2DOF": "TRCHI2DOF",
        "LOKI_MIPCHI2DV": "MIPCHI2DV(PRIMARY)",
        "LOKI_TRCHI2": "TRCHI2"
        }
    branch.addTool(LokiToolCommon)
    branch.ToolList += ['LoKi::Hybrid::TupleTool/LokiToolCommon']

# LoKi variables for event
LoKi_evt_vars = {
        # Track information
        "LOKI_nTracks": "RECSUMMARY(LHCb.RecSummary.nTracks, -1)",
        "LOKI_nLong": "RECSUMMARY(LHCb.RecSummary.nLongTracks, -1)",
        # RICH multiplicities
        "LOKI_nRICH1Hits": "switch(HASRECSUMMARY(20), RECSUMMARY(20, -1), -1)",
        "LOKI_nRICH2Hist": "switch(HASRECSUMMARY(21), RECSUMMARY(21, -1), -1)"
        }

ParentConfig(dtt.Bd)
ParentConfig(dtt.D0)
ParentConfig(dtt.Kstar)
CommonConfig(dtt.Bd)
CommonConfig(dtt.D0)
CommonConfig(dtt.Kstar)
CommonConfig(dtt.D0K)
CommonConfig(dtt.D0Pi)
CommonConfig(dtt.KstarK)
CommonConfig(dtt.KstarPi)

LoKi_evt = dtt.addTupleTool("LoKi::Hybrid::EvtTupleTool/LoKi_evt")
LoKi_evt.Preambulo = ['from LoKiTracks.decorators import *',
                      'from LoKiNumbers.decorators import *',
                      'from LoKiCore.functions import *']
LoKi_evt.VOID_Variables = LoKi_evt_vars

# ====
# Cone
# ====
Cone = dtt.Bd.addTupleTool("TupleToolTrackIsolation/Cone")
Cone.FillAsymmetry = True
Cone.MinConeAngle = 1.0
Cone.MaxConeAngle = 2.0
Cone.StepSize = 0.1
Cone.Verbose = True

# =================
# Configure DaVinci
# =================
DaVinci().UserAlgorithms += [dtt]
DaVinci().InputType = 'DST'
DaVinci().TupleFile = 'Tuple_Kpi.root'
DaVinci().PrintFreq = 1000
DaVinci().DataType = '2012'
DaVinci().Simulation = True
DaVinci().Lumi = False
DaVinci().EvtMax = -1  # Process all events
DaVinci().DDDBtag = 'dddb-20130929-1'
DaVinci().CondDBtag = 'sim-20130522-1-vc-md100'
