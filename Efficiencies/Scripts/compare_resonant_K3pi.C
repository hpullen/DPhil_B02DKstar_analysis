// Compare efficiencies for Kpipipi PHSP and resonant MC samples
void get_efficiency(TTree * tree) {

    // Set branch address of weight
    double weight;
    tree->SetBranchAddress("weight", &weight);

    // Sum weights before selection
    double sum_total = 0;
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        sum_total += weight;
    }

    // Sum weights after selection
    TCut cut = "BDTG_Kpipipi_run2 > 0 && abs(D0_M_doubleSwap_low - 1864.84) > 15"
        " && abs(D0_M_doubleSwap_high - 1864.84) > 15 && abs(D0_M - 1864.84) < 25"
        " && D0_FDS > 3 && abs(DK_mass - 5279.32) > 25";
    tree->Draw(">>elist", cut);
    TEventList * elist = (TEventList*)gDirectory->Get("elist");
    double sum_sel = 0;
    for (int i = 0; i < elist->GetN(); i++) {
        tree->GetEntry(elist->GetEntry(i));
        sum_sel += weight;
    }

    // Get efficiency and uncertainty
    double eff = sum_sel/sum_total;
    double err = (1/sum_total) * sqrt(sum_sel * (1 - sum_sel / sum_total));
    std::cout << eff << " +/- " << err << std::endl;

}


void compare_resonant_K3pi() {

    // Open samples
    TChain * phsp_tree = new TChain("DecayTree");
    TChain * res_tree = new TChain("DecayTree");
    TString dir = "/data/lhcb/users/pullen/B02DKstar/MC/";
    for (TString mag : {"_up", "_down"}) {
        phsp_tree->Add(dir + "fourBody/Kpipipi/2012" + mag + 
                "/Kpipipi_withBDTG_withWeights.root");
        res_tree->Add(dir + "backgrounds/Kpipipi_res/2012" + mag + 
                "/Kpipipi_withBDTG_withWeights.root");
    }

    // Get efficiency for each
    std::cout << "PHSP sample: ";
    get_efficiency(phsp_tree);
    std::cout << "Resonant sample: ";
    get_efficiency(res_tree);
}

