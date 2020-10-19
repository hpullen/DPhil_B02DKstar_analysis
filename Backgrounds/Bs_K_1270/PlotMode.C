inline double Product(const TLorentzVector& p1, const TLorentzVector& p2,
        const TLorentzVector& pX) {
    return - p1.Dot(p2) + p1.Dot(pX) * p2.Dot(pX) / pX.Dot(pX);
}

void LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
        const std::string& name) {
    tree->SetBranchAddress((name + "_E").c_str(), &vec[3]);
    tree->SetBranchAddress((name + "_PX").c_str(), &vec[0]);
    tree->SetBranchAddress((name + "_PY").c_str(), &vec[1]);
    tree->SetBranchAddress((name + "_PZ").c_str(), &vec[2]);
}

// =======================================================================
// Script to plot the B+ -> DK*0pi mode with predicted kinematic endpoints
// =======================================================================
void PlotMode(TString mode) {

    TFile * file = TFile::Open("/data/lhcb/users/pullen/RapidSim/Bs_K_1270/"
            + mode + ".root", "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");

    TH1F * hist = new TH1F("hist", "", 500, 2600, 5300);
    TCut mass_window = "Bd_M*1000 > 5000";
    double entries_before = tree->GetEntries(mass_window);
    std::cout << "Entries in region 5000-5800 before cuts: " << entries_before
        << std::endl;
    TCut cut = "abs(Kstar_M - 0.8955) < 0.05 && abs(D0_M - 1.86484) < 0.025";
    tree->Draw("Bd_M*1000>>hist", cut);
    double entries_after = tree->GetEntries(mass_window + cut);
    std::cout << "Entries in region 5000-5800 after K*0/D0 mass cuts: " << 
        entries_after << std::endl;
    std::cout << "(Ratio: " << entries_after/entries_before << ")\n\n";

    TCanvas * canvas = new TCanvas("canvas", "", 800, 400);
    hist->SetLineWidth(1);
    hist->Draw();
    TLine * line_low = new TLine(2760.65, 0, 2760.65, hist->GetMaximum());
    TLine * line_up = new TLine(5139.69, 0, 5139.69, hist->GetMaximum());
    line_low->SetLineStyle(2);
    line_low->SetLineColor(kRed);
    line_low->Draw();
    line_up->SetLineStyle(2);
    line_up->SetLineColor(kRed);
    line_up->Draw();
    canvas->SaveAs("Plots/" + mode + "B0_mass.pdf");

    // Calculate helicity angle
    TFile * newfile = TFile::Open("/data/lhcb/users/pullen/RapidSim/Bs_K_1270/"
            + mode + "_withHelicityAngle.root", "RECREATE");
    TTree * newtree = tree->CloneTree(0);
    double helicityAngle;
    double Kstar_M;
    double D0_M;
    double Bd_M;
    tree->SetBranchAddress("D0_M", &D0_M);
    tree->SetBranchAddress("Kstar_M", &Kstar_M);
    tree->SetBranchAddress("Bd_M", &Bd_M);
    newtree->Branch("helicity_angle", &helicityAngle, "helicity_angle/D");
    TLorentzVector D0_P;
    TLorentzVector KstarK_P;
    TLorentzVector KstarPi_P;
    LorentzVectorBranch(tree, D0_P, "D0b_0");
    LorentzVectorBranch(tree, KstarK_P, "Km_0");
    LorentzVectorBranch(tree, KstarPi_P, "pip_0");
    TH1F * hel_hist = new TH1F("hel_hist", "", 500, -1, 1);
    double entries_passing = 0;
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        helicityAngle = Product(KstarK_P, D0_P, KstarK_P + KstarPi_P) / 
            sqrt(Product(KstarK_P, KstarK_P, KstarK_P + KstarPi_P) * 
                    Product(D0_P, D0_P, KstarK_P + KstarPi_P));
        newtree->Fill();
        if (abs(D0_M - 1.86484) > 0.025) continue;
        if (abs(Kstar_M - 0.89551) > 0.050) continue;
        hel_hist->Fill(helicityAngle);
        if (Bd_M < 5) continue;
        if (abs(helicityAngle) < 0.4) continue;
        entries_passing++;
    }
    TCanvas * canv2 = new TCanvas("canv2", "", 500, 400);
    hel_hist->SetLineWidth(1);
    hel_hist->Draw("HIST");
    canv2->SaveAs("Plots/" + mode + "helicity_angle.pdf");
    std::cout << "Entries in region 5000-5800 after mass and helicity cuts: " << 
        entries_passing << std::endl;
    std::cout << "(Ratio: " << entries_passing/entries_before << ")\n\n";

    // Draw the mass within range
    TH1F * mass_hist = new TH1F("mass_hist", "", 100, 5000, 5800);
    TCut cut_hel = "abs(helicity_angle) > 0.4";
    newtree->Draw("Bd_M*1000>>mass_hist", cut + cut_hel);
    TCanvas * canv3 = new TCanvas("canv3", "", 900, 600);
    mass_hist->SetXTitle("#it{B}^{0} mass");
    mass_hist->SetYTitle("Entries / (8 MeV/#it{c}^{2})");
    mass_hist->SetLineWidth(1);
    mass_hist->Draw("HIST");
    TLine * line_Bmass = new TLine(5279.81, 0, 5279.81, mass_hist->GetMaximum());
    line_Bmass->SetLineColor(kRed);
    line_Bmass->SetLineStyle(2);
    line_Bmass->SetLineWidth(1);
    line_Bmass->Draw();
    canv3->SaveAs("Plots/B_mass_with_cuts_" + mode + ".pdf");


    // // Make Dalitz plot
    // TH2F * dalitz_hist = new TH2F("dalitz_hist", "", 100, 0, 15, 100, 0, 30);
    // tree->Draw("D0b_0_Kst0_0_M2_TRUE:Kst0_0_pip_0_M2_TRUE>>dalitz_hist");
    // // tree->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>dalitz_hist");
    // TCanvas * dal_canv = new TCanvas("dal_canv", "", 500, 500);
    // dalitz_hist->Draw("COLZ");
    // dal_canv->SaveAs("Plots/" + mode + "_dalitz.pdf");



}
