// =========================================================================
// Get background rejection and signal acceptance numbers for chosen BDT cut
// =========================================================================
void GetStats(bool weighted = false) {

    // Open files
    std::string ext = weighted ? "_weighted" : "";
    std::ofstream bkg_file("bkg_stats" + ext + ".txt");
    std::ofstream sig_file("sig_stats" + ext + ".txt");

    // Data and MC locations
    TString mc_path = "/data/lhcb/users/pullen/B02DKstar/MC/";
    TString data_path = "/data/lhcb/users/pullen/B02DKstar/data/";

    // Loop through BDT modes
    std::vector<std::string> modes = {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"};
    if (weighted) modes = {"Kpi"};
    for (TString mode : modes) {

        // Cut to apply
        TCut preselection = "abs(D0_M - 1864.84) < 25";
        TCut BKGCAT_cut = "Bd_BKGCAT == 0";
        TCut mass_cut = "Bd_ConsD_M > 5800";
        TCut BDT_cut;
        if (mode == "Kpi") {
            BDT_cut = ("BDTG_Kpi_run2" + ext + " > 0").c_str();
        } else if (mode== "KK") {
            BDT_cut = "BDTG_KK_run2 > 0.5";
        } else if (mode== "pipi") {
            BDT_cut = "BDTG_pipi_run2 > 0.7";
        } else if (mode== "Kpipipi") {
            BDT_cut = "BDTG_Kpipipi_run2 > 0";
        } else if (mode== "pipipipi") {
            BDT_cut = "BDTG_pipipipi_run2 > 0.7";
        }

        // Open MC
        TChain * mc_tree = new TChain("DecayTree");
        TChain * mc_tree_15 = new TChain("DecayTree");
        TChain * mc_tree_16 = new TChain("DecayTree");
        TString bod = (mode == "pipipipi" || mode == "Kpipipi") ?
            "fourBody" : "twoBody";
        for (TString mag : {"_up", "_down"}) {
            mc_tree->Add(mc_path + bod + "/" + mode + "/2016" + mag + "/"
                    + mode + "_withBDTG" + ext + ".root");
            mc_tree_16->Add(mc_path + bod + "/" + mode + "/2016" + mag + "/"
                    + mode + "_withBDTG" + ext + ".root");
            if (bod == "twoBody") {
                mc_tree->Add(mc_path + bod + "/" + mode + "/2015" + mag + "/"
                        + mode + "_withBDTG" + ext + ".root");
                mc_tree_15->Add(mc_path + bod + "/" + mode + "/2015" + mag + "/"
                        + mode + "_withBDTG" + ext + ".root");
            }
        }

        // Get signal acceptance
        double mc_total = mc_tree->GetEntries(preselection + BKGCAT_cut);
        double mc_final = mc_tree->GetEntries(preselection + BKGCAT_cut + BDT_cut);
        double err_mc =1/mc_total * sqrt(mc_final * (1- mc_final/mc_total));
        sig_file << mode << " " << mc_final/mc_total << " " << err_mc << std::endl;
        double mc_total_16 = mc_tree_16->GetEntries(preselection + BKGCAT_cut);
        double mc_final_16 = mc_tree_16->GetEntries(preselection + BKGCAT_cut + BDT_cut);
        double err_mc_16 =1/mc_total_16 * sqrt(mc_final_16 * (1- mc_final_16/mc_total_16));
        sig_file << mode << "_2016 " << mc_final_16/mc_total_16 << " " << err_mc_16 << std::endl;
        if (bod == "twoBody") {
            double mc_total_15 = mc_tree_15->GetEntries(preselection + BKGCAT_cut);
            double mc_final_15 = mc_tree_15->GetEntries(preselection + BKGCAT_cut + BDT_cut);
            double err_mc_15 =1/mc_total_15 * sqrt(mc_final_15 * (1- mc_final_15/mc_total_15));
            sig_file << mode << "_2015 " << mc_final_15/mc_total_15 << " " << err_mc_15 << std::endl;
        }

        // Open data
        TChain * data_tree = new TChain("DecayTree");
        TChain * data_tree_15 = new TChain("DecayTree");
        TChain * data_tree_16 = new TChain("DecayTree");
        for (TString mag : {"_up", "_down"}) {
            data_tree->Add(data_path + bod + "/2016" + mag + "/"
                    + mode + "_withBDTG" + ext + ".root");
            data_tree_16->Add(data_path + bod + "/2016" + mag + "/"
                    + mode + "_withBDTG" + ext + ".root");
            data_tree->Add(data_path + bod + "/2015" + mag + "/"
                    + mode + "_withBDTG" + ext + ".root");
            data_tree_15->Add(data_path + bod + "/2015" + mag + "/"
                    + mode + "_withBDTG" + ext + ".root");
        }

        // Get background rejection
        double data_total = data_tree->GetEntries(preselection + mass_cut);
        double data_final = data_tree->GetEntries(preselection + mass_cut + BDT_cut);
        double err_data =1/data_total * sqrt(data_final * (1- data_final/data_total));
        bkg_file << mode << " " << 1 - data_final/data_total << " " << err_data << std::endl;
        double data_total_15 = data_tree_15->GetEntries(preselection + mass_cut);
        double data_final_15 = data_tree_15->GetEntries(preselection + mass_cut + BDT_cut);
        double err_data_15 =1/data_total_15 * sqrt(data_final_15 * (1- data_final_15/data_total_15));
        bkg_file << mode << "_2015 " << 1 - data_final_15/data_total_15 << " " << err_data_15 << std::endl;
        double data_total_16 = data_tree_16->GetEntries(preselection + mass_cut);
        double data_final_16 = data_tree_16->GetEntries(preselection + mass_cut + BDT_cut);
        double err_data_16 =1/data_total_16 * sqrt(data_final_16 * (1- data_final_16/data_total_16));
        bkg_file << mode << "_2016 " << 1 - data_final_16/data_total_16 << " " << err_data_16 << std::endl;
    }

    sig_file.close();
    bkg_file.close();

}
