import ROOT

# Get fraction of yields in different L0 catgories for data and MC
# Loop through modes
for mode in ["Kpi", "Kpipipi"]:

    # Two or four body
    bod = "fourBody" if (len(mode) > 4) else "twoBody"

    # Open file
    f = open("L0_fractions/{}.param".format(mode), "w+")

    # Loop through years
    years = ["2012", "2016"]
    if mode == "Kpi":
        years.append("2011")
        years.append("2015")
    for year in years:

        # Open data and MC trees
        print "Processing {} {}".format(mode, year)
        data_tree = ROOT.TChain("DecayTree")
        mc_tree = ROOT.TChain("DecayTree")
        path = "/data/lhcb/users/pullen/B02DKstar/"
        for mag in ["up", "down"]:
            data_tree.Add("{}/data/{}/{}_{}/{}_selected_withWeights.root".format(path, bod, year, mag, mode))
            mc_tree.Add("{}/MC/{}/{}/{}_{}/{}_selected.root".format(path, bod, mode, year, mag, mode))

        # Get data fractions
        data_hadron_sum = 0
        data_total_sum = 0
        for evt, err in enumerate(data_tree):
            data_total_sum += data_tree.sw_signal
            if data_tree.Bd_L0HadronDecision_TOS and not data_tree.Bd_L0Global_TIS:
                data_hadron_sum += data_tree.sw_signal
        data_frac = data_hadron_sum / data_total_sum

        # Get MC fractions
        mc_hadron_sum = 0
        mc_total_sum = 0
        mc_hadron_sum_weighted = 0
        mc_total_sum_weighted = 0
        mc_hadron_sum_weighted_BDT = 0
        mc_total_sum_weighted_BDT = 0
        for evt, err in enumerate(mc_tree):
            mc_total_sum += 1.0
            mc_total_sum_weighted += mc_tree.weight
            mc_total_sum_weighted_BDT += mc_tree.weight_BDT_vars
            if mc_tree.Bd_L0HadronDecision_TOS and not mc_tree.Bd_L0Global_TIS:
                mc_hadron_sum += 1.0
                mc_hadron_sum_weighted += mc_tree.weight
                mc_hadron_sum_weighted_BDT += mc_tree.weight_BDT_vars
        mc_frac = mc_hadron_sum / mc_total_sum
        mc_frac_weighted = mc_hadron_sum_weighted / mc_total_sum_weighted
        mc_frac_weighted_BDT = mc_hadron_sum_weighted_BDT / mc_total_sum_weighted_BDT

        # Print to file
        f.write("{}_data {}\n".format(year, data_frac))
        f.write("{}_MC {}\n".format(year, mc_frac))
        f.write("{}_MC_weighted {}\n".format(year, mc_frac_weighted))
        f.write("{}_MC_weighted_BDT {}\n".format(year, mc_frac_weighted_BDT))

    # Close file
    f.close()









