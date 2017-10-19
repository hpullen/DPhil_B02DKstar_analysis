# ===================================================================================
# DaVinci for 2015 data with KK and pipi modes only; decay descriptor typos corrected
# ===================================================================================

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
from Configurables import TrackScaleState, CondDB
from Configurables import LoKi__Hybrid__TupleTool, LoKi__Hybrid__EvtTupleTool
from DecayTreeTuple.Configuration import *

# ========================================
# Stream and stripping
# ========================================
stream = 'Bhadron'
line = 'B02D0KPiD2HHBeauty2CharmLine'

# =================
# Decay descriptors
# =================
dec_Kpi = ('[[B0 -> ^(D0 -> ^K+ ^pi-) ^(K*(892)0 -> ^K+ ^pi-)]CC,'
           '[B0 -> ^(D0 -> ^K- ^pi+) ^(K*(892)~0 -> ^K- ^pi+)]CC]')
dec_piK = ('[[B0 -> ^(D0 -> ^K- ^pi+) ^(K*(892)0 -> ^K+ ^pi-)]CC,'
           '[B0 -> ^(D0 -> ^K+ ^pi-) ^(K*(892)~0 -> ^K- ^pi+)]CC]')
dec_KK = ('[[B0 -> ^(D0 -> ^K+ ^K-) ^(K*(892)0 -> ^K+ ^pi-)]CC,'
          '[B0 -> ^(D0 -> ^K+ ^K-) ^(K*(892)~0 -> ^K- ^pi+)]CC]')
dec_pipi = ('[[B0 -> ^(D0 -> ^pi+ ^pi-) ^(K*(892)0 -> ^K+ ^pi-)]CC,'
            '[B0 -> ^(D0 -> ^pi+ ^pi-) ^(K*(892)~0 -> ^K- ^pi+)]CC]')

# ======================
# Set up DecayTreeTuples
# ======================
tuple_Kpi = DecayTreeTuple('Tuple_Kpi')
tuple_piK = DecayTreeTuple('Tuple_piK')
tuple_KK = DecayTreeTuple('Tuple_KK')
tuple_pipi = DecayTreeTuple('Tuple_pipi')

tupleList = [tuple_Kpi, tuple_piK, tuple_KK, tuple_pipi]

for ntp in tupleList:
    ntp.Inputs = ['/Phys/{1}/Particles'.format(stream, line)]
    ntp.RootInTES = '/Event/{0}'.format(stream)

# ==================
# Assign decay modes
# ==================
tuple_Kpi.Decay = dec_Kpi
tuple_piK.Decay = dec_piK
tuple_KK.Decay = dec_KK
tuple_pipi.Decay = dec_pipi

# ============
# Add Branches
# ============
tuple_Kpi.addBranches({
    "Bd":      ('[[B0 -> (D0 -> K+ pi-) (K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> (D0 -> K- pi+) (K*(892)~0 -> K- pi+)]CC]'),
    "D0":      ('[[B0 -> ^(D0 -> K+ pi-) (K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> ^(D0 -> K- pi+) (K*(892)~0 -> K- pi+)]CC]'),
    "D0K":     ('[[B0 -> (D0 -> ^K+ pi-) (K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> (D0 -> ^K- pi+) (K*(892)~0 -> K- pi+)]CC]'),
    "D0Pi":    ('[[B0 -> (D0 -> K+ ^pi-) (K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> (D0 -> K- ^pi+) (K*(892)~0 -> K- pi+)]CC]'),
    "Kstar":   ('[[B0 -> (D0 -> K+ pi-) ^(K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> (D0 -> K- pi+) ^(K*(892)~0 -> K- pi+)]CC]'),
    "KstarK":  ('[[B0 -> (D0 -> K+ pi-) (K*(892)0 -> ^K+ pi-)]CC,'
                '[B0 -> (D0 -> K- pi+) (K*(892)~0 -> ^K- pi+)]CC]'),
    "KstarPi": ('[[B0 -> (D0 -> K+ pi-) (K*(892)0 -> K+ ^pi-)]CC,'
                '[B0 -> (D0 -> K- pi+) (K*(892)~0 -> K- ^pi+)]CC]'),
    })

tuple_piK.addBranches({
    "Bd":      ('[[B0 -> (D0 -> K- pi+) (K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> (D0 -> K+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0":      ('[[B0 -> ^(D0 -> K- pi+) (K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> ^(D0 -> K+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0K":     ('[[B0 -> (D0 -> ^K- pi+) (K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> (D0 -> ^K+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0Pi":    ('[[B0 -> (D0 -> K- ^pi+) (K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> (D0 -> K+ ^pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "Kstar":   ('[[B0 -> (D0 -> K- pi+) ^(K*(892)0 -> K+ pi-)]CC,'
                '[B0 -> (D0 -> K+ pi-) ^(K*(892)~0 -> K- pi+)]CC]'),
    "KstarK":  ('[[B0 -> (D0 -> K- pi+) (K*(892)0 -> ^K+ pi-)]CC,'
                '[B0 -> (D0 -> K+ pi-) (K*(892)~0 -> ^K- pi+)]CC]'),
    "KstarPi": ('[[B0 -> (D0 -> K- pi+) (K*(892)0 -> K+ ^pi-)]CC,'
                '[B0 -> (D0 -> K+ pi-) (K*(892)~0 -> K- ^pi+)]CC]'),
    })

tuple_KK.addBranches({
    "Bd":       ('[[B0 -> (D0 -> K- K+) (K*(892)0 -> K+ pi-)]CC,'
                 '[B0 -> (D0 -> K+ K-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0":       ('[[B0 -> ^(D0 -> K- K+) (K*(892)0 -> K+ pi-)]CC,'
                 '[B0 -> ^(D0 -> K+ K-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0Kplus":  ('[[B0 -> (D0 -> ^K+ K-) (K*(892)0 -> K+ pi-)]CC,'
                 '[B0 -> (D0 -> ^K+ K-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0Kminus": ('[[B0 -> (D0 -> K+ ^K-) (K*(892)0 -> K+ pi-)]CC,'
                 '[B0 -> (D0 -> K+ ^K-) (K*(892)~0 -> K- pi+)]CC]'),
    "Kstar":    ('[[B0 -> (D0 -> K- K+) ^(K*(892)0 -> K+ pi-)]CC,'
                 '[B0 -> (D0 -> K+ K-) ^(K*(892)~0 -> K- pi+)]CC]'),
    "KstarK":   ('[[B0 -> (D0 -> K- K+) (K*(892)0 -> ^K+ pi-)]CC,'
                 '[B0 -> (D0 -> K+ K-) (K*(892)~0 -> ^K- pi+)]CC]'),
    "KstarPi":  ('[[B0 -> (D0 -> K- K+) (K*(892)0 -> K+ ^pi-)]CC,'
                 '[B0 -> (D0 -> K+ K-) (K*(892)~0 -> K- ^pi+)]CC]'),
    })

tuple_pipi.addBranches({
    "Bd":        ('[[B0 -> (D0 -> pi- pi+) (K*(892)0 -> K+ pi-)]CC,'
                  '[B0 -> (D0 -> pi+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0":        ('[[B0 -> ^(D0 -> pi- pi+) (K*(892)0 -> K+ pi-)]CC,'
                  '[B0 -> ^(D0 -> pi+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0PiPlus":  ('[[B0 -> (D0 -> ^pi+ pi-) (K*(892)0 -> K+ pi-)]CC,'
                  '[B0 -> (D0 -> ^pi+ pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "D0PiMinus": ('[[B0 -> (D0 -> pi+ ^pi-) (K*(892)0 -> K+ pi-)]CC,'
                  '[B0 -> (D0 -> pi+ ^pi-) (K*(892)~0 -> K- pi+)]CC]'),
    "Kstar":     ('[[B0 -> (D0 -> pi- pi+) ^(K*(892)0 -> K+ pi-)]CC,'
                  '[B0 -> (D0 -> pi+ pi-) ^(K*(892)~0 -> K- pi+)]CC]'),
    "KstarK":    ('[[B0 -> (D0 -> pi- pi+) (K*(892)0 -> ^K+ pi-)]CC,'
                  '[B0 -> (D0 -> pi+ pi-) (K*(892)~0 -> ^K- pi+)]CC]'),
    "KstarPi":   ('[[B0 -> (D0 -> pi- pi+) (K*(892)0 -> K+ ^pi-)]CC,'
                  '[B0 -> (D0 -> pi+ pi-) (K*(892)~0 -> K- ^pi+)]CC]'),
    })

# ===============================
# Group charged daughter branches
# ===============================
Kpi_h = [tuple_Kpi.D0K, tuple_Kpi.D0Pi, tuple_Kpi.KstarK, tuple_Kpi.KstarPi]
piK_h = [tuple_piK.D0K, tuple_piK.D0Pi, tuple_piK.KstarK, tuple_piK.KstarPi]
KK_h = [tuple_KK.D0Kplus, tuple_KK.D0Kminus, tuple_KK.KstarK, tuple_KK.KstarPi]
pipi_h = [tuple_pipi.D0PiPlus, tuple_pipi.D0PiMinus, tuple_pipi.KstarK,
          tuple_pipi.KstarPi]
branchList_h = Kpi_h + piK_h + KK_h + pipi_h

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
for ntp in tupleList:
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

    # TupleToolStripping
    ttstripping = ntp.addTupleTool("TupleToolStripping/ttstripping")
    ttstripping.StrippingList = [("StrippingB02D0KPiD2HH"
                                  "Beauty2CharmLineDecision")]

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
LoKi_Bd_extra_vars = {
        "LV01":        "LV01",
        "LV02":        "LV02",
        "ptasy_1.50": ("RELINFO('/Event/Bhadron/Phys/"
                       "B02D0KPiD2HHBeauty2CharmLine/P2ConeVar1',"
                       "'CONEPTASYM', -1000.)"),
        "ptasy_1.70": ("RELINFO('/Event/Bhadron/Phys/"
                       "B02D0KPiD2HHBeauty2CharmLine/P2ConeVar2',"
                       "'CONEPTASYM', -1000.)")
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
for branch in branchList_h:
    LoKi_h = branch.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_h")
    LoKi_h.Preambulo = ["from LoKiTracks.decorators import *"]
    LoKi_h.Variables = LoKi_h_vars

for ntp in tupleList:
    LoKi_Bd = ntp.Bd.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_Bd")
    LoKi_Bd.Variables = LoKi_Bd_vars

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
scaler = TrackScaleState('StateScale')
scaler.RootInTES = '/Event/{0}/'.format(stream) 

# =================
# Configure DaVinci
# =================
DaVinci().RootInTES = '/Event/{0}'.format(stream)
DaVinci().InputType = 'MDST'
DaVinci().TupleFile = 'Tuple_Bd_DKstar.root'
DaVinci().PrintFreq = 1000
year = '2015'
DaVinci().DataType = year
DaVinci().Simulation = False
DaVinci().Lumi = True
DaVinci().EvtMax = -1
CondDB(LatestGlobalTagByDataType = year)
DaVinci().appendToMainSequence([scaler])

# ================================
# Add tuples to list of algorithms
# ================================
DaVinci().UserAlgorithms += tupleList
