// =====================================================
// Script to make tree with all background contributions
// =====================================================
void MakeTotalTree() {

    // List of modes
    std::vector<std::string> modes = {
        "1_Kstar_1680/Kstar",
        "1_Kstar_1680/rho",
        "2_Kstar_892",
        "3_K1_1400/Kstar",
        "3_K1_1400/rho",
        "3_K1_1400/non_res",
        "4_Kstar_1430",
        "5_Kstar_1410/Kstar",
        "5_Kstar_1410/rho",
        "6_rho",
        "7_K1_1270/Kstar",
        "7_K1_1270/rho",
        "10_nonres"
    };

    // Setup tree
    TFile * treefile = TFile::Open("/data/lhcb/users/pullen/RapidSim/DKpipi/total.root", "RECREATE");
    TTree * outtree = new TTree("DecayTree", "");
    double Bd_M_out;
    outtree->Branch("Bd_M", &Bd_M_out, "Bd_M/D");

    // Cut to apply to masses
    TCut cut = "abs(Kstar_M - 0.8955) < 0.05 && abs(D0_M - 1.86484) < 0.025" 
        " && abs(helicity_angle) > 0.4";

    // Loop through modes
    for (auto mode : modes) {

        // Open tree
        TFile * file = TFile::Open(("/data/lhcb/users/pullen/RapidSim/DKpipi/" + 
                mode + "/tree.root").c_str(), "READ");
        TTree * tree = (TTree*)file->Get("SelectedTree");

        // Apply mass cuts
        tree->Draw(">> elist", cut);
        TEventList * elist = (TEventList*)gDirectory->Get("elist");

        // Mass branch
        double Bd_M;
        tree->SetBranchAddress("Bd_M", &Bd_M);

        // Calculate angle and fill tree
        for (int i = 0; i < elist->GetN(); i++) {
            int evt = elist->GetEntry(i);
            tree->GetEntry(evt);
            Bd_M_out = Bd_M * 1000;
            outtree->Fill();
        }
        file->Close();
    }

    // Save tree
    treefile->cd();
    outtree->Write("DecayTree");
    treefile->Close();

}
