// Add ETA variable to TTree
void add_misID_mass(TString mag) {

    // Open file
    TFile * infile = TFile::Open("/data/lhcb/users/pullen/B02DKstar/MC/"
            "backgrounds/Dpipi/2016_" + mag + "/Kpi_selected_withPIDweights.root",
            "READ");
    TTree * intree = (TTree*)infile->Get("DecayTree");

    // Set branch addresses
    double KstarK_PX, KstarK_PY, KstarK_PZ, KstarK_PE;
    double KstarPi_PX, KstarPi_PY, KstarPi_PZ, KstarPi_PE;
    double KstarK_PIDK, KstarPi_PIDK;
    double D0_PX, D0_PY, D0_PZ, D0_PE;
    double B_M;
    float B_ConstD0_M;
    intree->SetBranchAddress("Xdau_p_PX", &KstarK_PX);
    intree->SetBranchAddress("Xdau_p_PY", &KstarK_PY);
    intree->SetBranchAddress("Xdau_p_PZ", &KstarK_PZ);
    intree->SetBranchAddress("Xdau_p_PE", &KstarK_PE);
    intree->SetBranchAddress("Xdau_p_PIDK", &KstarK_PIDK);
    intree->SetBranchAddress("Xdau_m_PX", &KstarPi_PX);
    intree->SetBranchAddress("Xdau_m_PY", &KstarPi_PY);
    intree->SetBranchAddress("Xdau_m_PZ", &KstarPi_PZ);
    intree->SetBranchAddress("Xdau_m_PE", &KstarPi_PE);
    intree->SetBranchAddress("Xdau_m_PIDK", &KstarPi_PIDK);
    intree->SetBranchAddress("D0_PX", &D0_PX);
    intree->SetBranchAddress("D0_PY", &D0_PY);
    intree->SetBranchAddress("D0_PZ", &D0_PZ);
    intree->SetBranchAddress("D0_PE", &D0_PE);
    intree->SetBranchAddress("B_M", &B_M);
    intree->SetBranchAddress("B_ConstD0_M", &B_ConstD0_M);

    // Make new tree
    TFile * outfile = TFile::Open("/data/lhcb/users/pullen/B02DKstar/MC/"
            "backgrounds/Dpipi/2016_" + mag + "/Kpi_selected_withMisIDmass.root",
            "RECREATE");
    TTree * outtree = (TTree*)intree->CloneTree(0);

    // Make new branches
    double misID_M = 0;
    outtree->Branch("misID_M", &misID_M, "misID_M/D");
    double B_ConstD0_MD = 0;
    outtree->Branch("B_ConstD0_MD", &B_ConstD0_MD, "B_ConstD0_MD/D");

    // Apply loose NN cut
    TCut cut = "NN>0 && NN1>0";
    intree->Draw(">>elist", cut);
    TEventList * elist = (TEventList*)gDirectory->Get("elist");
    std::cout << "Looping over " << elist->GetN() << " entries." << std::endl;

    // Loop and fill tree
    double B0_M, KstarK_Psq, KstarK_newE;
    double m_K = 493.677;
    double m_Kstar = 895.55;
    TLorentzVector D0_vec, KstarK_vec, KstarPi_vec, Kstar_vec, B0_vec;
    for (int i = 0; i < elist->GetN(); i++) {

        // Get entry
        intree->GetEntry(elist->GetEntry(i));
        if (i % 10000 == 0) {
            std::cout << "Processing event " << i << std::endl;
        }

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

        // Fill new tree
        if (abs(Kstar_vec.M() - m_Kstar) < 50) {
            misID_M = B0_vec.M();
            B_ConstD0_MD = (double)B_ConstD0_M;
            outtree->Fill();
        }
    }

    // Save and close
    outfile->cd();
    outtree->Write("DecayTree");
    outfile->Close();
    infile->Close();

}
