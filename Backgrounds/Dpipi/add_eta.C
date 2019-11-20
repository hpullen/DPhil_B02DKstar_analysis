// Add ETA variable to TTree
void add_eta(TString mag) {

    // Open file
    TFile * infile = TFile::Open("/data/lhcb/users/pullen/B02DKstar/Mark_tuple/"
            "B02Dpipi_D2hh_kpi_Run2_trim_split_forNeuroBayes_NN_NN.root", "READ");
    TTree * intree = (TTree*)infile->Get("DecayTreekpi");

    // Make new tree
    TFile * outfile = TFile::Open("/data/lhcb/users/pullen/B02DKstar/Mark_tuple/"
            "Kpi_" + mag + "_withETA.root", "RECREATE");
    TTree * outtree = (TTree*)intree->CloneTree(0);

    // Set branch addresses
    int Ddau_p_ID;
    intree->SetBranchAddress("Ddau_p_ID", &Ddau_p_ID);
    double Ddau_p_PZ, Ddau_p_P;
    intree->SetBranchAddress("Ddau_p_PZ", &Ddau_p_PZ);
    intree->SetBranchAddress("Ddau_p_P", &Ddau_p_P);
    double Ddau_m_PZ, Ddau_m_P;
    intree->SetBranchAddress("Ddau_m_PZ", &Ddau_m_PZ);
    intree->SetBranchAddress("Ddau_m_P", &Ddau_m_P);
    double Xdau_p_PZ, Xdau_p_P;
    intree->SetBranchAddress("Xdau_p_PZ", &Xdau_p_PZ);
    intree->SetBranchAddress("Xdau_p_P", &Xdau_p_P);
    double Xdau_m_PZ, Xdau_m_P;
    intree->SetBranchAddress("Xdau_m_PZ", &Xdau_m_PZ);
    intree->SetBranchAddress("Xdau_m_P", &Xdau_m_P);

    // Make new branches
    double KstarK_P, KstarK_PZ, KstarK_ETA;
    outtree->Branch("KstarK_P", &KstarK_P, "KstarK_P/D");
    outtree->Branch("KstarK_PZ", &KstarK_PZ, "KstarK_PZ/D");
    outtree->Branch("KstarK_ETA", &KstarK_ETA, "KstarK_ETA/D");
    double KstarPi_P, KstarPi_PZ, KstarPi_ETA;
    outtree->Branch("KstarPi_P", &KstarPi_P, "KstarPi_P/D");
    outtree->Branch("KstarPi_PZ", &KstarPi_PZ, "KstarPi_PZ/D");
    outtree->Branch("KstarPi_ETA", &KstarPi_ETA, "KstarPi_ETA/D");
    double D0K_P, D0K_PZ, D0K_ETA;
    outtree->Branch("D0K_P", &D0K_P, "D0K_P/D");
    outtree->Branch("D0K_PZ", &D0K_PZ, "D0K_PZ/D");
    outtree->Branch("D0K_ETA", &D0K_ETA, "D0K_ETA/D");
    double D0Pi_P, D0Pi_PZ, D0Pi_ETA;
    outtree->Branch("D0Pi_P", &D0Pi_P, "D0Pi_P/D");
    outtree->Branch("D0Pi_PZ", &D0Pi_PZ, "D0Pi_PZ/D");
    outtree->Branch("D0Pi_ETA", &D0Pi_ETA, "D0Pi_ETA/D");

    // Apply loose NN cut
    TCut cut = "NN>0 && NN1>0";
    if (mag == "up" || mag == "MagUp") {
        cut += "Polarity>0";
    } else if (mag == "down" || mag == "MagDown") {
        cut += "Polarity<0";
    } else {
        std::cout << "Unrecognised polarity: " << mag << std::endl;
        return;
    }
        
    intree->Draw(">>elist", cut);
    TEventList * elist = (TEventList*)gDirectory->Get("elist");
    std::cout << "Looping over " << elist->GetN() << " entries for polarity " << mag << "." << std::endl;

    // Loop and fill tree
    for (int i = 0; i < elist->GetN(); i++) {

        // Get entry
        intree->GetEntry(elist->GetEntry(i));
        if (i % 10000 == 0) {
            std::cout << "Processing event " << i << std::endl;
        }

        // Assign momenta
        if (Ddau_p_ID == 321) {
            D0K_P = Ddau_p_P;
            D0K_PZ = Ddau_p_PZ;
            D0Pi_P = Ddau_m_P;
            D0Pi_PZ = Ddau_m_PZ;
            KstarK_P = Xdau_p_P;
            KstarK_PZ = Xdau_p_PZ;
            KstarPi_P = Xdau_m_P;
            KstarPi_PZ = Xdau_m_PZ;
        } else {
            D0K_P = Ddau_m_P;
            D0K_PZ = Ddau_m_PZ;
            D0Pi_P = Ddau_p_P;
            D0Pi_PZ = Ddau_p_PZ;
            KstarK_P = Xdau_m_P;
            KstarK_PZ = Xdau_m_PZ;
            KstarPi_P = Xdau_p_P;
            KstarPi_PZ = Xdau_p_PZ;
        }

        // Calculate ETA
        D0K_ETA = atanh(D0K_PZ/fabs(D0K_P));
        D0Pi_ETA = atanh(D0Pi_PZ/fabs(D0Pi_P));
        KstarK_ETA = atanh(KstarK_PZ/fabs(KstarK_P));
        KstarPi_ETA = atanh(KstarPi_PZ/fabs(KstarPi_P));

        // Fill new tree
        outtree->Fill();
    }

    // Save and close
    outfile->cd();
    outtree->Write();
    outfile->Close();
    infile->Close();

}
