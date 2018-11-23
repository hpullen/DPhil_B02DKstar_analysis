// Get efficiency of FDS cuts on signal MC
void GetEfficiency() {

    for (TString mode : {"Kpi", "Kpipipi"}) {

        // Make cuts
        TString BDT_cut = "BDTG_" + mode + "_run2 > 0";
        TCut cut = TCut(BDT_cut);
        cut += "abs(D0_M - 1864.84) < 25";
        cut += "abs(DK_mass - 5279.32) > 25";
        cut += "Bd_BKGCAT == 0";
        if (mode == "Kpi") {
            cut += "abs(D0_M_doubleSwap - 1864.84 > 15)";
        } else {
            cut += "abs(D0_M_doubleSwap_low - 1864.84 > 15)";
            cut += "abs(D0_M_doubleSwap_high - 1864.84 > 15)";
        }
        TCut cut_FDS = cut + "D0_FDS > 3";

        // Open MC
        TChain * chain = new TChain("DecayTree");
        TString bod = (mode == "Kpi") ? "twoBody" : "fourBody";
        for (TString mag : {"_down", "_up"}) {
            chain->Add("/data/lhcb/users/pullen/B02DKstar/MC/" + bod
                    + "/" + mode + "/2016" + mag + "/" + mode + "_withBDTG.root");
        }

        // Get efficiency
        double entries = chain->GetEntries(cut);
        double cut_entries = chain->GetEntries(cut_FDS);
        double ratio = cut_entries/entries;
        std::cout << "Efficiency for " << mode << ": " << ratio * 100 
            << " %" << std::endl;
    }
}
