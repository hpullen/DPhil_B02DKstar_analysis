import ROOT
import sys
#  import array
#  import math
#  import os
from hep_ml.reweight import GBReweighter
import numpy as np


## FUNCTION TO GET VARIABLE WITHOUT OPERATION ##
def get_raw_var(var):
    match = next((name for name in ["log10(", "acos("] if name in var), False)
    return var[len(match):len(var) - 1] if match else var


### FUNCTION TO GET OPERATION FROM VARIABLE NAME ###
def get_func(var):
    def log_func(v):
        return np.log10(v)
    def acos_func(v):
        return np.arccos(v)
    def default_func(v):
        return v
    funcs = {
            "log10(" : log_func,
            "acos(" : acos_func
            }
    match = next((name for name in funcs.keys() if name in var), False)
    return funcs[match] if match else default_func


### OPEN INPUTS ###
# Get year to process and 2-body/4-body
year = sys.argv[1]
bod = sys.argv[2]
if year in {"2011", "2015"} and bod == "4":
    quit()

# Open input data/MC trees
mc_tree = ROOT.TChain("DecayTree")
data_tree = ROOT.TChain("DecayTree")
bodies = "twoBody" if bod == "2" else "fourBody"
input_mode = "Kpi" if bod == "2" else "fourBody"
base_path = "/data/lhcb/users/pullen/B02DKstar/"
for mag in ["up", "down"]:
    mc_tree.Add("{base_path}/MC/{bodies}/{mode}/{year}_{mag}/{mode}_selected.root".format(base_path=base_path, 
            bodies=bodies, mode=input_mode, year=year, mag=mag))
    data_tree.Add("{base_path}/data/{bodies}/{year}_{mag}/{mode}_selected_withWeights.root".format(base_path=base_path, 
            bodies=bodies, mode=input_mode, year=year, mag=mag))


### LISTS OF BRANCHES ###
simple_list = np.array(["Bd_PT", "Bd_ETA"])
BDT_file = open("../../BDT/VariableLists/{}_run2.txt".format(input_mode), "r")
BDT_list = np.array(BDT_file.read().splitlines())
BDT_file.close()
branch_lists = {
        "weight" : simple_list,
        "weight_BDT_vars" : BDT_list
        }

### MAKE REWEIGHTERS ###
reweighters = {name : GBReweighter(max_depth=2, gb_args={'subsample': 0.5}) for name in branch_lists.keys()}
for name, rw in reweighters.iteritems():
    n_branches = len(branch_lists[name])
    mc_data = real_data = {}
    real_weights = np.empty([data_tree.GetEntries()])
    mc_data = np.empty([mc_tree.GetEntries(), n_branches])
    real_data = np.empty([data_tree.GetEntries(), n_branches])
    for tree, data in {mc_tree : mc_data, data_tree : real_data}.iteritems():
        for evt, err in enumerate(tree):
            data[evt] = [get_func(var)(getattr(tree, get_raw_var(var) if not (var == "Bd_ETA" and year in ["2011", "2012"] and tree == mc_tree) else "Bd_LOKI_ETA")) 
                    for var in branch_lists[name]]
            if tree is data_tree:
                real_weights[evt] = tree.sw_signal
    rw.fit(original=mc_data, target=real_data, target_weight=real_weights)


### APPLY REWEIGHTING TO MC ###
# Locations of data to include
locs = {"{}/{}".format(bodies, input_mode) : input_mode}
if bod == "2":
    locs["backgrounds/Bs"] = "Kpi";
    locs["twoBody/Kpi"] = "Kpi";
    if year != "2011":
        locs["twoBody/KK"] = "KK";
        locs["twoBody/pipi"] = "pipi";
else:
    if year == "2016":
        locs["fourBody/pipipipi"] = "pipipipi"

# Make output tree
for loc, mode in locs.iteritems():
    for mag in ["up", "down"]:
        input_tree = ROOT.TChain("DecayTree")
        full_path = "{base_path}/MC/{loc}/{year}_{mag}/".format(base_path=base_path, 
            loc=loc, year=year, mag=mag)
        print "Adding weights to file in {}".format(full_path)
        input_tree.Add("{}/{}_withBDTG.root".format(full_path, mode))

        # Branch name conversions (if needed)
        conversions = {}
        if year in ["2011", "2012"]:
            conversions["Bd_ETA"] = "Bd_LOKI_ETA"
        if mode in ["KK", "pipi"]:
            swaps = {"D0K": "D0{}lus".format("Kp" if mode == "KK" else "PiP"),
                    "D0Pi": "D0{}inus".format("Km" if mode == "KK" else "PiM")
                    }
            conversions.update({"{}_{}".format(to_swap, end) : "{}_{}".format(swapped, end) for to_swap, swapped in swaps.iteritems() for end in ["PT", "IPCHI2"]})

        # Get output weights
        output_weights = {}
        for name, rw in reweighters.iteritems():
            input_data = np.empty([input_tree.GetEntries(), len(branch_lists[name])])
            for evt, err in enumerate(input_tree):
                converted = [conversions.get(var, var) for var in branch_lists[name]]
                input_data[evt] = [get_func(var)(getattr(input_tree, get_raw_var(var))) for var in converted]
            output_weights[name] = rw.predict_weights(input_data)

        # Fill new tree
        output_file = ROOT.TFile("{}/{}_withWeights.root".format(full_path, mode), "RECREATE")
        output_tree = input_tree.CloneTree(0)
        new_branches = {name : np.zeros(1, dtype=float) for name in reweighters.keys()}
        for name in new_branches.keys():
            output_tree.Branch(name, new_branches[name], "{}/D".format(name)) 
        for evt, err in enumerate(input_tree):
            input_tree.GetEntry(evt)    
            for name in new_branches.keys():
                new_branches[name] = output_weights[name][evt]
            output_tree.Fill()

        # Save and close
        output_file.cd()
        output_tree.Write("DecayTree")
        output_file.Close()
