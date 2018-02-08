// ===================================================
// Plot all components of DKpipi background normalized
// ===================================================
void PlotScaled() {

    // Open files and get trees
    TString dir = "/data/lhcb/users/pullen/RapidSim/B_DKpipi_study/";

    // 1. B+ -> K*(1680)+ D0
    TFile * file_1a = TFile::Open(dir + "1_K_1680/Kstar/tree.root", "READ");
    TTree * tree_1a = (TTree*)file_1a->Get("DecayTree");
    TFile * file_1b = TFile::Open(dir + "1_K_1680/rho_K/tree.root", "READ");
    TTree * tree_1b = (TTree*)file_1b->Get("DecayTree");

    // 2. B+ -> K*0 D0 pi+
    TFile * file_2 = TFile::Open(dir + "2_Kstar/tree.root", "READ");
    TTree * tree_2 = (TTree*)file_2->Get("DecayTree");

    // 3. B+ -> K1(1400)+ D0
    TFile * file_3 = TFile::Open(dir + "3_K1_1400/tree.root", "READ");
    TTree * tree_3 = (TTree*)file_3->Get("DecayTree");

    // 4. B+ -> K*0(1430)0 D0
    TFile * file_4 = TFile::Open(dir + "4_K0_1430/tree.root", "READ");
    TTree * tree_4 = (TTree*)file_4->Get("DecayTree");

    // 5. B+ -> K*(1410)+ D0
    TFile * file_5a = TFile::Open(dir + "5_K_1410/Kstar/tree.root", "READ");
    TTree * tree_5a = (TTree*)file_5a->Get("DecayTree");
    TFile * file_5b = TFile::Open(dir + "5_K_1410/rho_K/tree.root", "READ");
    TTree * tree_5b = (TTree*)file_5b->Get("DecayTree");

    // 6. B+ -> rho0 D0 K+
    TFile * file_6 = TFile::Open(dir + "6_K_rho/tree.root", "READ");
    TTree * tree_6 = (TTree*)file_6->Get("DecayTree");

    // 7. B+ -> K1(1270)+ D0
    TFile * file_7a = TFile::Open(dir + "7_K1_1270/Kstar/tree.root", "READ");
    TTree * tree_7a = (TTree*)file_7a->Get("DecayTree");
    TFile * file_7b = TFile::Open(dir + "7_K1_1270/rho_K/tree.root", "READ");
    TTree * tree_7b = (TTree*)file_7b->Get("DecayTree");

    // Ignore 8 and 9 for now: opposite sign decays

    // 10. Non-resonant B+ -> K+ pi- pi+ D0
    TFile * file_10 = TFile::Open(dir + "10_nonres/tree.root", "READ");
    TTree * tree_10 = (TTree*)file_10->Get("DecayTree");

    // Make cut
    TCut cut = "abs(Kstar_M - 0.8955) < 0.05 && Bd_M > 5";

    // Make histograms
    double max_Bd_M = 5.3;
    TH1F * hist_1a = new TH1F("hist_1a", "", 100, 5, max_Bd_M);
    TH1F * hist_1b = new TH1F("hist_1b", "", 100, 5, max_Bd_M);
    TH1F * hist_2 = new TH1F("hist_2", "", 100, 5, max_Bd_M);
    TH1F * hist_3 = new TH1F("hist_3", "", 100, 5, max_Bd_M);
    TH1F * hist_4 = new TH1F("hist_4", "", 100, 5, max_Bd_M);
    TH1F * hist_5a = new TH1F("hist_5a", "", 100, 5, max_Bd_M);
    TH1F * hist_5b = new TH1F("hist_5b", "", 100, 5, max_Bd_M);
    TH1F * hist_6 = new TH1F("hist_6", "", 100, 5, max_Bd_M);
    TH1F * hist_7a = new TH1F("hist_7a", "", 100, 5, max_Bd_M);
    TH1F * hist_7b = new TH1F("hist_7b", "", 100, 5, max_Bd_M);
    TH1F * hist_10 = new TH1F("hist_10", "", 100, 5, max_Bd_M);

    // Fill histograms
    tree_1a->Draw("Bd_M>>hist_1a", cut);
    tree_1b->Draw("Bd_M>>hist_1b", cut);
    tree_2->Draw("Bd_M>>hist_2", cut);
    tree_3->Draw("Bd_M>>hist_3", cut);
    tree_4->Draw("Bd_M>>hist_4", cut);
    tree_5a->Draw("Bd_M>>hist_5a", cut);
    tree_5b->Draw("Bd_M>>hist_5b", cut);
    tree_6->Draw("Bd_M>>hist_6", cut);
    tree_7a->Draw("Bd_M>>hist_7a", cut);
    tree_7b->Draw("Bd_M>>hist_7b", cut);
    tree_10->Draw("Bd_M>>hist_10", cut);

    // Set colours
    hist_1a->SetLineWidth(1);
    hist_1a->SetLineColor(kRed);
    hist_1b->SetLineWidth(1);
    hist_1b->SetLineStyle(2);
    hist_1b->SetLineColor(kRed);
    hist_2->SetLineWidth(1);
    hist_2->SetLineColor(kBlue);
    hist_3->SetLineWidth(1);
    hist_3->SetLineColor(kOrange);
    hist_4->SetLineWidth(1);
    hist_4->SetLineColor(kYellow);
    hist_5a->SetLineWidth(1);
    hist_5a->SetLineColor(kCyan);
    hist_5b->SetLineWidth(1);
    hist_5b->SetLineStyle(2);
    hist_5b->SetLineColor(kCyan);
    hist_6->SetLineWidth(1);
    hist_6->SetLineColor(kViolet);
    hist_7a->SetLineWidth(1);
    hist_7a->SetLineColor(kMagenta);
    hist_7b->SetLineWidth(1);
    hist_7b->SetLineStyle(2);
    hist_7b->SetLineColor(kMagenta);
    hist_10->SetLineWidth(1);
    hist_10->SetLineColor(kGreen);

    // Normalize histograms
    hist_1a->Scale(0.175 * 0.67 * 0.755 * 1/hist_1a->Integral());
    hist_1b->Scale(0.175 * 0.33 * 0.104 * 1/hist_1b->Integral());
    hist_2->Scale(0.175 * 0.746 * 1/hist_2->Integral());
    hist_3->Scale(0.14 * 0.762 * 1/hist_3->Integral());
    hist_4->Scale(0.13 * 0.0185 * 1/hist_4->Integral());
    hist_5a->Scale(0.12 * 0.9 * 0.76 * 1/hist_5a->Integral());
    hist_5b->Scale(0.12 * 0.1 * 0.177 * 1/hist_5b->Integral());
    hist_6->Scale(0.11 * 0.056 * 1/hist_6->Integral());
    hist_7a->Scale(0.05 * (7 / 21) * 0.773 * 1/hist_7a->Integral());
    hist_7b->Scale(0.05 * (14 / 21) * 0.185 * 1/hist_7b->Integral());
    hist_10->Scale(0.03 * 0.0517 * 1/hist_10->Integral());

    // Labels
    hist_1a->GetXaxis()->SetTitle("Reconstructed B^{0} mass / GeV");
    hist_1a->GetYaxis()->SetTitle("Scaled entries");

    // Make legend
    TLegend * leg = new TLegend(0.55, 0.2, 0.85, 0.9);
    leg->AddEntry(hist_1a, "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    leg->AddEntry(hist_1b, "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}");
    leg->AddEntry(hist_2, "B^{+} #rightarrow K^{*0} #bar{D}^{0} #pi^{+}");
    leg->AddEntry(hist_3, "B^{+} #rightarrow (K_{1}(1400)^{+} #rightarrow K^{*0} "
            "#pi^{+}) #bar{D}^{0}");
    leg->AddEntry(hist_4, "B^{+} #rightarrow K_{0}^{*}(1430)^{0} #pi^{+} "
            "#bar{D}^{0}");
    leg->AddEntry(hist_5a, "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    leg->AddEntry(hist_5b, "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}");
    leg->AddEntry(hist_6, "B^{+} #rightarrow K^{+} #rho^{0} #bar{D}^{0}");
    leg->AddEntry(hist_7a, "B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    leg->AddEntry(hist_7b, "B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}");
    leg->AddEntry(hist_10, "B^{+} #rightarrow K^{+} #pi^{-} #pi^{+} #bar{D}^{0}");

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 800, 400);
    canvas->cd();
    hist_1a->Draw();
    hist_1b->Draw("SAME");
    hist_2->Draw("SAME");
    hist_3->Draw("SAME");
    hist_4->Draw("SAME");
    hist_5a->Draw("SAME");
    hist_5b->Draw("SAME");
    hist_6->Draw("SAME");
    hist_7a->Draw("SAME");
    hist_7b->Draw("SAME");
    hist_10->Draw("SAME");
    leg->Draw();

    // Save
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/Backgrounds/DKpipi/Plots/"
            "components_normalized.pdf");

}
