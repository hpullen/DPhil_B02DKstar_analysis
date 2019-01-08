// ===================================================
// Plot all components of DKpipi background normalized
// ===================================================
void PlotDalitzMC() {

    // Open file and get tree
    TString dir = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/DKpipi/";
    TFile * file = TFile::Open(dir + "all_simulations_selected.root");
    TTree * tree = (TTree*)file->Get("DecayTree");

    // Set branch addresses
    double KstarK_PE, KstarK_PX, KstarK_PY, KstarK_PZ;
    tree->SetBranchAddress("KstarK_TRUEP_E", &KstarK_PE);
    tree->SetBranchAddress("KstarK_TRUEP_X", &KstarK_PX);
    tree->SetBranchAddress("KstarK_TRUEP_Y", &KstarK_PY);
    tree->SetBranchAddress("KstarK_TRUEP_Z", &KstarK_PZ);
    double KstarPi_PE, KstarPi_PX, KstarPi_PY, KstarPi_PZ;
    tree->SetBranchAddress("KstarPi_TRUEP_E", &KstarPi_PE);
    tree->SetBranchAddress("KstarPi_TRUEP_X", &KstarPi_PX);
    tree->SetBranchAddress("KstarPi_TRUEP_Y", &KstarPi_PY);
    tree->SetBranchAddress("KstarPi_TRUEP_Z", &KstarPi_PZ);
    double D0_PE, D0_PX, D0_PY, D0_PZ;
    tree->SetBranchAddress("D0_TRUEP_E", &D0_PE);
    tree->SetBranchAddress("D0_TRUEP_X", &D0_PX);
    tree->SetBranchAddress("D0_TRUEP_Y", &D0_PY);
    tree->SetBranchAddress("D0_TRUEP_Z", &D0_PZ);
    double Bd_PE, Bd_PX, Bd_PY, Bd_PZ;
    tree->SetBranchAddress("Bd_TRUEP_E", &Bd_PE);
    tree->SetBranchAddress("Bd_TRUEP_X", &Bd_PX);
    tree->SetBranchAddress("Bd_TRUEP_Y", &Bd_PY);
    tree->SetBranchAddress("Bd_TRUEP_Z", &Bd_PZ);

    // Make histograms
    TH1F * hist_DKpi = new TH1F("hist_DKpi", "", 100, 5000, 5200);
    TH1F * hist_Kpipi = new TH1F("hist_Kpipi", "", 100, 500, 2500);
    TH2F * hist_both = new TH2F("hist_both", "", 100, 500, 2500, 100, 4000, 6000);

    // Background category
    int cat = 0;
    tree->SetBranchAddress("Bd_BKGCAT", &cat);

    // Loop through and get 3-body invariant mass
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        if (cat != 40 && cat != 50) continue;

        // Get Lorentz vectors
        TLorentzVector KstarK_P(KstarK_PX, KstarK_PY, KstarK_PZ, KstarK_PE);
        TLorentzVector KstarPi_P(KstarPi_PX, KstarPi_PY, KstarPi_PZ, KstarPi_PE);
        TLorentzVector D0_P(D0_PX, D0_PY, D0_PZ, D0_PE);
        TLorentzVector Bd_P(Bd_PX, Bd_PY, Bd_PZ, Bd_PE);
        TLorentzVector pi_P = Bd_P - D0_P - KstarK_P - KstarPi_P;

        // Get invariant masses
        double inv_DKpi = (D0_P + KstarK_P + KstarPi_P).M();
        double inv_Kpipi = (KstarK_P + pi_P + KstarPi_P).M();

        // Fill histograms
        hist_DKpi->Fill(inv_DKpi);
        hist_Kpipi->Fill(inv_Kpipi);
        hist_both->Fill(inv_Kpipi, inv_DKpi);
    }

    // Plot and save
    TCanvas * canv_1D = new TCanvas("canv_1D", "", 900, 600);
    hist_DKpi->Draw("E");
    canv_1D->SaveAs("../Plots/DKpi_inv_mass.pdf");
    canv_1D->Clear();
    hist_Kpipi->Draw();
    canv_1D->SaveAs("../Plots/Kpipi_inv_mass.pdf");
    TCanvas * canv_2D = new TCanvas("canv_2D", "", 900, 900);
    hist_both->Draw("COLZ");
    canv_2D->SaveAs("../Plots/dalitz_MC.pdf");
}
