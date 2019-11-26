import ROOT
import sys
#  import array
#  import math
#  import os
from hep_ml.reweight import GBReweighter
import numpy as np


## FUNCTION TO GET VARIABLE WITHOUT OPERATION ##
def get_raw_var(var):
    def log_func(v):
        return np.log10(v)

    def acos_func(v):
        return np.arccos(v)

    def default_func(v):
        return v

    for op in ["log10(", "acos("]:
        if var.find(op) != -1:
            var_short = var[len(op):len(var)-1]
            op_type = log_func if op == "log10(" else acos_func
            return var_short, op_type
    return var, default_func


### OPEN INPUTS ###
# Get year to process and 2-body/4-body
year = sys.argv[1]
bod = sys.argv[2]
if (year == "2011" or year == "2015") and bod == "4":
    quit()
full_tuple = True
if len(sys.argv) == 4:
    if (sys.argv[3] == "--full"):
        full_tuple = True
    else:
        print "Unrecognised option: ", sys.argv[3]
        quit()


# Open input data/MC trees
mc_tree = ROOT.TChain("DecayTree")
data_tree = ROOT.TChain("DecayTree")
for mag in ["up", "down"]:
    if bod is "2":
        mc_file = "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/{}_{}/Kpi_selected.root".format(year, mag)
        data_file = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{}_{}/Kpi_selected_withWeights.root".format(year, mag)
    else:
        mc_file = "/data/lhcb/users/pullen/B02DKstar/MC/fourBody/Kpipipi/{}_{}/Kpipipi_selected.root".format(year, mag)
        data_file = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{}_{}/Kpipipi_selected_withWeights.root".format(year, mag)
    mc_tree.Add(mc_file)
    data_tree.Add(data_file)


### SET UP BRANCHES ###
# List of simple weighting variables
branch_list = np.array(["Bd_PT", "Bd_ETA"])

# List of BDT variables
branch_list_BDT = np.array(["log10(Bd_IPCHI2_OWNPV)", "acos(Bd_DIRA_OWNPV)", 
    "Bd_ptasy_1.50", "log10(KstarK_IPCHI2_OWNPV)", 
    "log10(KstarPi_IPCHI2_OWNPV)", "log10(KstarK_PT)", "log10(KstarPi_PT)",
    "Bd_LOKI_VFASPF_VCHI2VDOF", "log10(D0_IPCHI2_OWNPV)"])
if bod == "2":
    extras = np.array(["log10(D0K_PT)", "log10(D0Pi_PT)", 
        "log10(D0K_IPCHI2_OWNPV)", "log10(D0Pi_IPCHI2_OWNPV)"])
    branch_list_BDT = np.concatenate([branch_list_BDT, extras])

# Both lists
full_list = np.concatenate([branch_list, branch_list_BDT])
raw_names = {}
op_types = {}
for name in full_list:
    raw_name, op = get_raw_var(name)
    raw_names[name] = raw_name
    op_types[name] = op

# Set branch addresses of variables to weight
mc_branches = {}
data_branches = {}
for branch in full_list:

    # Set MC branch address
    mc_branches[branch] = np.zeros(1, dtype=float)
    if branch == "Bd_ETA" and (year == "2011" or year == "2012"):
        mc_tree.SetBranchAddress("Bd_LOKI_ETA", mc_branches[branch])
    else:
        mc_tree.SetBranchAddress(raw_names[branch], mc_branches[branch])

    # Set data branch address
    data_branches[branch] = np.zeros(1, dtype=float)
    data_tree.SetBranchAddress(raw_names[branch], data_branches[branch])

# Set up sWeight branch
sWeight = np.zeros(1, dtype=float)
data_tree.SetBranchAddress("sw_signal", sWeight)


### MAKE REWEIGHTERS ###
# Make reweighter for each reweighting scheme
branch_lists = {
        "weight" : branch_list,
        "weight_BDT_vars" : branch_list_BDT
        }
reweighters = {}
for blist in branch_lists.keys():
    reweighters[blist] = GBReweighter(max_depth=2, gb_args={'subsample': 0.5})

# Loop over reweighters and make each
for name, rw in reweighters.iteritems():

    # Make arrays
    current_list = branch_lists[name]
    mc_data = np.empty([mc_tree.GetEntries(), len(current_list)])
    real_data = np.empty([data_tree.GetEntries(), len(current_list)])
    real_weights = np.empty([len(real_data)])

    # Fill MC data
    for evt, err in enumerate(mc_tree):
        mc_tree.GetEntry(evt)
        for b in range(len(current_list)):
            bname = current_list[b]
            mc_data[evt, b] = op_types[bname](mc_branches[bname])

    # Fill real data and sWeights
    for evt, err in enumerate(data_tree):
        data_tree.GetEntry(evt)
        real_weights[evt] = sWeight
        for b in range(len(current_list)):
            bname = current_list[b]
            real_data[evt, b] = op_types[bname](data_branches[bname])

    # Fit reweighter
    rw.fit(original=mc_data, target=real_data, target_weight=real_weights)


### APPLY REWEIGHTING TO MC ###
# Loop over files to apply the weights to
locs = {}
if bod is "2":
    #  locs["backgrounds/Bs"] = "Kpi";
    locs["twoBody/Kpi"] = "Kpi";
    locs["twoBody/KK"] = "KK";
    locs["twoBody/pipi"] = "pipi";
    #  if year == "2016":
        #  locs["backgrounds/rho"] = "Kpi"
        #  for particle in ["gamma", "pi"]:
            #  for helicity in ["010", "001", "100"]:
                #  locs["backgrounds/rho_lowMass/{}/{}".format(particle, helicity)] = "Kpi"
else:
    #  locs["backgrounds/Kpipipi_res"] = "Kpipipi"
    locs["fourBody/Kpipipi"] = "Kpipipi"
    locs["fourBody/pipipipi"] = "pipipipi"

for loc, mode in locs.iteritems():
    for mag in ["up", "down"]:

        if year == "2018" and mag == "down" and "KK" in loc:
            continue

        # Open the input file
        input_tree = ROOT.TChain("DecayTree")
        filename = "/data/lhcb/users/pullen/B02DKstar/MC/{}/{}_{}/{}_withBDTG.root".format(loc, year, mag, mode)
        if full_tuple:
            filename = "/data/lhcb/users/pullen/B02DKstar/MC/{}/{}_{}/{}_full.root".format(loc, year, mag, mode)
        print "Adding weights to file {}".format(filename)
        input_tree.Add(filename)

        # Make output tree
        output_name = "/data/lhcb/users/pullen/B02DKstar/MC/{}/{}_{}/{}_withBDTG_withWeights.root".format(loc, year, mag, mode)
        if full_tuple:
            output_name = "/data/lhcb/users/pullen/B02DKstar/MC/{}/{}_{}/{}_withWeights.root".format(loc, year, mag, mode)
        print 'Outputting file to {}'.format(output_name)
        output_file = ROOT.TFile(output_name, "RECREATE")
        output_tree = input_tree.CloneTree(0)

        # Branch conversions (if needed)
        conversions = {}
        if year == "2011" or year == "2012":
            conversions["Bd_ETA"] = "Bd_LOKI_ETA"
        if mode == "KK" or mode == "pipi":
            d1 = "D0Kplus" if mode == "KK" else "D0PiPlus"
            d2 = "D0Kminus" if mode == "KK" else "D0PiMinus"
            for end in ["PT", "IPCHI2_OWNPV"]:
                conversions["D0K_" + end] = d1 + "_" + end
                conversions["D0Pi_" + end] = d2 + "_" + end

        # Set branches
        input_branches = {}
        for branch in full_list:
            input_branches[branch] = np.zeros(1, dtype=float)
            branch_name = raw_names[branch]
            if branch_name in conversions:
                branch_name = conversions[branch_name]
            input_tree.SetBranchAddress(branch_name, 
                    input_branches[branch])

        # Make arrays of data
        input_data = {}
        for name, blist in branch_lists.iteritems():
            input_data[name] = np.empty([input_tree.GetEntries(), len(blist)])
        for evt, err in enumerate(input_tree):
            input_tree.GetEntry(evt)
            for name, blist in branch_lists.iteritems():
                for b in range(len(blist)):
                    bname = blist[b]
                    input_data[name][evt, b] = op_types[bname](input_branches[bname])

        # Get weights and make branches
        output_weights = {}
        weights = {}
        for name, rw in reweighters.iteritems():
            output_weights[name] = rw.predict_weights(input_data[name])
            weights[name] = np.zeros(1, dtype=float)
            output_tree.Branch(name, weights[name], "{}/D".format(name))

        # Apply weights
        for evt, err in enumerate(input_tree):
            input_tree.GetEntry(evt)
            for name in branch_lists.keys():
                weights[name][0] = output_weights[name][evt]
            output_tree.Fill()

        # Save and close
        output_file.cd()
        output_tree.Write("DecayTree")
        output_file.Close()
