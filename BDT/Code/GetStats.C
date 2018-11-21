// =========================================================================
// Get background rejection and signal acceptance numbers for chosen BDT cut
// =========================================================================
void GetStats() {

    // Open files
    std::ofstream bkg_file("bkg_stats.txt");
    std::ofstream sig_file("sig_stats.txt");

    // Data and MC locations
    TString mc_path = "/data/lhcb/users/pullen/B02DKstar/MC/";
    TString data_path = "/data/lhcb/users/pullen/B02DKstar/data/";

    // Loop through BDT modes
    for (TString mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {

        // Cut to apply
        TCut preselection = "abs(D0_M - 1864.84) < 25";
        TCut BKGCAT_cut = "Bd_BKGCAT == 0";
        TCut mass_cut = "Bd_ConsD_M > 5800";
        TCut BDT_cut;
        if (mode == "Kpi") {
            BDT_cut = "BDTG_Kpi_run2 > 0";
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
        TString bod = (mode == "pipipipi" || mode == "Kpipipi") ?
            "fourBody" : "twoBody";
        for (TString mag : {"_up", "_down"}) {
            mc_tree->Add(mc_path + bod + "/" + mode + "/2016" + mag + "/"
                    + mode + "_withBDTG.root");
            if (bod == "twoBody") {
                mc_tree->Add(mc_path + bod + "/" + mode + "/2015" + mag + "/"
                        + mode + "_withBDTG.root");
            }
        }

        // Get signal acceptance
        double mc_total = mc_tree->GetEntries(preselection + BKGCAT_cut);
        double mc_final = mc_tree->GetEntries(preselection + BKGCAT_cut + BDT_cut);
        sig_file << mode << " " << mc_final/mc_total << std::endl;

        // Open data
        TChain * data_tree = new TChain("DecayTree");
        for (TString mag : {"_up", "_down"}) {
            data_tree->Add(data_path + bod + "/2016" + mag + "/"
                    + mode + "_withBDTG.root");
            data_tree->Add(data_path + bod + "/2015" + mag + "/"
                    + mode + "_withBDTG.root");
        }

        // Get background rejection
        double data_total = data_tree->GetEntries(preselection + mass_cut);
        double data_final = data_tree->GetEntries(preselection + mass_cut + BDT_cut);
        bkg_file << mode << " " << 1 - data_final/data_total << std::endl;
    }

    sig_file.close();
    bkg_file.close();

}
