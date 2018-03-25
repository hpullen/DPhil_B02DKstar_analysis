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

    TFile * file = TFile::Open("/data/lhcb/users/pullen/RapidSim/B_DKpipi_study/"
            "with_evtGen/" + mode + "_tree.root", "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");

    TH1F * hist = new TH1F("hist", "", 500, 2600, 5300);
    TCut cut = "abs(Kstar_M - 0.8955) < 0.05 && abs(D0_M - 1.86484) < 0.025";
    tree->Draw("Bd_M*1000>>hist", cut);

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
    canvas->SaveAs("../Plots/" + mode + "B0_mass.pdf");

    // Plot helicity angle
    double helicityAngle;
    double Kstar_M;
    double D0_M;
    tree->SetBranchAddress("D0_M", &D0_M);
    tree->SetBranchAddress("Kstar_M", &Kstar_M);
    TLorentzVector D0_P;
    TLorentzVector KstarK_P;
    TLorentzVector KstarPi_P;
    LorentzVectorBranch(tree, D0_P, "D0b_0");
    LorentzVectorBranch(tree, KstarK_P, "Kp_1");
    LorentzVectorBranch(tree, KstarPi_P, "pim_1");
    TH1F * hel_hist = new TH1F("hel_hist", "", 500, -1, 1);
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        if (abs(D0_M - 1.86484) > 0.025) continue;
        if (abs(Kstar_M - 0.89551) > 0.050) continue;
        helicityAngle = Product(KstarK_P, D0_P, KstarK_P + KstarPi_P) / 
            sqrt(Product(KstarK_P, KstarK_P, KstarK_P + KstarPi_P) * 
                    Product(D0_P, D0_P, KstarK_P + KstarPi_P));
        hel_hist->Fill(helicityAngle);
    }
    TCanvas * canv2 = new TCanvas("canv2", "", 500, 400);
    hel_hist->SetLineWidth(1);
    hel_hist->Draw("HIST");
    canv2->SaveAs("../Plots/" + mode + "helicity_angle.pdf");

    // Make Dalitz plot
    TH2F * dalitz_hist = new TH2F("dalitz_hist", "", 100, 0, 15, 100, 0, 30);
    tree->Draw("D0b_0_Kst0_0_M2_TRUE:Kst0_0_pip_0_M2_TRUE>>dalitz_hist");
    // tree->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>dalitz_hist");
    TCanvas * dal_canv = new TCanvas("dal_canv", "", 500, 500);
    dalitz_hist->Draw("COLZ");
    dal_canv->SaveAs("../Plots/" + mode + "_dalitz.pdf");



}
