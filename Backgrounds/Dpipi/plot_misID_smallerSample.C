void plot_misID_smallerSample() {

    // Open data
    TFile * file = TFile::Open("/data/lhcb/users/pullen/B02DKstar/Mark_tuple/"
            "B02Dpipi_D2hh_kpi_Run2_trim_split_forNeuroBayes_NN_NN.root", "READ");
    TTree * tree = (TTree*)file->Get("DecayTreekpi");

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
    tree->SetBranchStatus("NN", 1);
    tree->SetBranchStatus("NN1", 2);

    // Set branch addresses
    double KstarK_PX, KstarK_PY, KstarK_PZ, KstarK_PE;
    double KstarPi_PX, KstarPi_PY, KstarPi_PZ, KstarPi_PE;
    double KstarK_PIDK, KstarPi_PIDK;
    double D0_PX, D0_PY, D0_PZ, D0_PE;
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

    // Select only events with good NN output
    // tree->Draw(">>elist", "NN>0.5 && NN1>0.4");
    // tree->Draw(">>elist", "NN>-1 && NN1>-1");
    tree->Draw(">>elist", "NN>0 && NN1>0");
    TEventList * elist = (TEventList*)gDirectory->Get("elist");
    std::cout << "NN cuts: " << elist->GetN() << " entries" << std::endl;

    // Fill histograms with misID invariant mass
    TH1F * hist = new TH1F("hist", "", 100, 5000, 5800);
    TH1F * hist_PIDcut = new TH1F("hist_PIDcut", "", 100, 5000, 5800);
    double B0_M, KstarK_Psq, KstarK_newE;
    double m_K = 493.677;
    double m_Kstar = 895.55;
    TLorentzVector D0_vec, KstarK_vec, KstarPi_vec, Kstar_vec, B0_vec;
    for (int i = 0; i < elist->GetN(); i++) {

        // Get entry
        tree->GetEntry(elist->GetEntry(i));

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
        B0_vec = D0_vec + KstarK_vec;

        if (i % 10000 == 0) {
            // std::cout << "Processing event " << i << std::endl;
            // std::cout << "KstarK mass: " << KstarK_vec.M() << std::endl;
            // std::cout << "KstarPi mass: " << KstarPi_vec.M() << std::endl;
            // std::cout << "Kstar mass: " << Kstar_vec.M() << std::endl;
            // std::cout << "B0 mass: " << B0_vec.M() << std::endl;
            // std::cout << "D0 mass: " << D0_vec.M() << std::endl;
        }

        // Put cut on Kstar mass
        if (abs(Kstar_vec.M() - m_Kstar) < 50) {

            // Fill uncut histogram
            hist->Fill(B0_vec.M());

            // Fill PID cut histogram
            if (KstarK_PIDK > 5 && KstarPi_PIDK < -1) {
                hist_PIDcut->Fill(B0_vec.M());
            }
        }
    }

    // Plot histograms
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    hist->GetXaxis()->SetTitle("misID B mass / MeV");
    hist_PIDcut->GetXaxis()->SetTitle("misID B mass / MeV");
    hist->Draw();
    canvas->SaveAs("misID_B_mass_noPID_clean.pdf");
    canvas->Clear();
    hist_PIDcut->Draw();
    canvas->SaveAs("misID_B_mass_PIDcut_clean.pdf");

    // Save histograms
    TFile * output = TFile::Open("misID_B_mass_histos_clean.root", "RECREATE");
    hist->Write("B_mass");
    hist_PIDcut->Write("B_mass_PIDcut");
    output->Close();
}

