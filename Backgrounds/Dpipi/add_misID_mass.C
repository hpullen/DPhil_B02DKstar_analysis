void add_misID_mass() {

    // Open data
    gROOT->ForceStyle();
    TChain * tree = new TChain("DecayTree");
    tree->Add("/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Dpipi/"
            "2016_down/Kpi_selected_withPIDweights.root");
    tree->Add("/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Dpipi/"
            "2016_up/Kpi_selected_withPIDweights.root");

    // Make output tree
    TFile * file = new TFile("/data/lhcb/users/pullen/B02DKstar/MC/"
            "backgrounds/Kpi_withPIDweights_withMisID.root", "RECREATE");
    TTree * newtree = (TTree*)tree->CloneTree(0);

    // Turn on relevant branches only
    tree->SetBranchStatus("*", 0);
    for (TString p : {"D0", "Xdau_p", "Xdau_m"}) {
        for (TString v : {"_PE", "_PX", "_PY", "_PZ"}) {
            tree->SetBranchStatus(p + v, 1);
        }
        if (p != "D0") {
            tree->SetBranchStatus(p + "_PIDK", 1);
        }
    }
    tree->SetBranchStatus("B_M", 1);
    tree->SetBranchStatus("NN", 1);
    tree->SetBranchStatus("NN1", 1);
    tree->SetBranchStatus("PID_efficiency", 1);

    // Set branch addresses
    double KstarK_PX, KstarK_PY, KstarK_PZ, KstarK_PE;
    double KstarPi_PX, KstarPi_PY, KstarPi_PZ, KstarPi_PE;
    double KstarK_PIDK, KstarPi_PIDK;
    double D0_PX, D0_PY, D0_PZ, D0_PE;
    double B_M;
    tree->SetBranchAddress("Xdau_p_PX", &KstarK_PX);
    tree->SetBranchAddress("Xdau_p_PY", &KstarK_PY);
    tree->SetBranchAddress("Xdau_p_PZ", &KstarK_PZ);
    tree->SetBranchAddress("Xdau_p_PE", &KstarK_PE);
    tree->SetBranchAddress("Xdau_p_PIDK", &KstarK_PIDK);
    tree->SetBranchAddress("Xdau_m_PX", &KstarPi_PX);
    tree->SetBranchAddress("Xdau_m_PY", &KstarPi_PY);
    tree->SetBranchAddress("Xdau_m_PZ", &KstarPi_PZ);
    tree->SetBranchAddress("Xdau_m_PE", &KstarPi_PE);
    tree->SetBranchAddress("Xdau_m_PIDK", &KstarPi_PIDK);
    tree->SetBranchAddress("D0_PX", &D0_PX);
    tree->SetBranchAddress("D0_PY", &D0_PY);
    tree->SetBranchAddress("D0_PZ", &D0_PZ);
    tree->SetBranchAddress("D0_PE", &D0_PE);
    tree->SetBranchAddress("B_M", &B_M);

    // Make new branch
    double misID_M;
    newtree->Branch("misID_M", &misID_M, "misID_M/D");

    // Fill tree with misID invariant mass
    double B0_M, KstarK_Psq, KstarK_newE;
    double m_K = 493.677;
    double m_Kstar = 895.55;
    TLorentzVector D0_vec, KstarK_vec, KstarPi_vec, Kstar_vec, B0_vec;
    for (int i = 0; i < tree->GetEntries(); i++) {

        // Get entry
        tree->GetEntry(i);

        // Recalculate misID kaon energy
        KstarK_Psq = KstarK_PX*KstarK_PX + KstarK_PY*KstarK_PY + 
            KstarK_PZ*KstarK_PZ;
        KstarK_newE = sqrt(KstarK_Psq + m_K*m_K);

        // Make vectors
        KstarK_vec = TLorentzVector(KstarK_PX, KstarK_PY, KstarK_PZ, KstarK_newE);
        KstarPi_vec = TLorentzVector(KstarPi_PX, KstarPi_PY, KstarPi_PZ, 
                KstarPi_PE);
        D0_vec = TLorentzVector(D0_PX, D0_PY, D0_PZ, D0_PE);
        Kstar_vec = KstarK_vec + KstarPi_vec;
        B0_vec = D0_vec + Kstar_vec;

        if (i % 10000 == 0) {
            std::cout << "Processing event " << i << std::endl;
            // std::cout << "KstarK mass: " << KstarK_vec.M() << std::endl;
            // std::cout << "KstarPi mass: " << KstarPi_vec.M() << std::endl;
            // std::cout << "Kstar mass: " << Kstar_vec.M() << std::endl;
            // std::cout << "B0 mass: " << B0_vec.M() << std::endl;
            // std::cout << "D0 mass: " << D0_vec.M() << std::endl;
        }

        // Fill tree
        misID_M = B0_vec.M();
        newtree->Fill();
    }

    // Save tree
    file->cd();
    newtree->Write("DecayTree");
    file->Close();

}


