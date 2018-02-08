// ===================================================
// Plot all components of DKpipi background normalized
// ===================================================
void PlotWithMC() {

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

    // Open Monte Carlo
    TFile * file_MC = TFile::Open("/data/lhcb/users/pullen/B02DKstar/MC/"
            "backgrounds/DKpipi/all_simulations_selected.root", "READ");
    TTree * tree_MC = (TTree*)file_MC->Get("DecayTree");

    // Make cuts
    TString cut = "(abs(Kstar_M - 0.8955) < 0.05 && Bd_M > 5)";
    TString cut_MC = "abs(D0_M - 1864.84) < 25 && D0_FDS > 2"
        "&& BDTG_Kpi_run2 > 0.5 && (Bd_BKGCAT == 40 || Bd_BKGCAT == 50) &&"
        "Bd_ConsD_M < 5160";

    // Make histograms
    double max_Bd_M = 5.3;
    TH1F * hist_rs = new TH1F("hist_rs", "", 100, 5, max_Bd_M);
    TH1F * hist_MC = new TH1F("hist_MC", "", 100, 5, max_Bd_M);

    // Fill RapidSim histogram
    tree_1a->Draw("Bd_M>>hist_rs", cut + " * 0.175 * 0.67 * 0.755"); 
    tree_1b->Draw("Bd_M>>hist_rs", cut + " * 0.175 * 0.33 * 0.104");
    tree_2->Draw("Bd_M>>hist_rs", cut + " * 0.175 * 0.746");
    tree_3->Draw("Bd_M>>hist_rs", cut + " * 0.14 * 0.762");
    tree_4->Draw("Bd_M>>hist_rs", cut + " * 0.13 * 0.0185");
    tree_5a->Draw("Bd_M>>hist_rs", cut + " * 0.12 * 0.9 * 0.76");
    tree_5b->Draw("Bd_M>>hist_rs", cut + " * 0.12 * 0.1 * 0.177");
    tree_6->Draw("Bd_M>>hist_rs", cut + " * 0.11 * 0.056");
    tree_7a->Draw("Bd_M>>hist_rs", cut + " * 0.05 * (7/21) * 0.773");
    tree_7b->Draw("Bd_M>>hist_rs", cut + " * 0.05 * (14/21) * 0.185");
    tree_10->Draw("Bd_M>>hist_rs", cut + " * 0.03 * 0.0517");

    // Fill MC histogram
    tree_MC->Draw("(Bd_ConsD_M / 1000)>>hist_MC", cut_MC);

    // Scale
    hist_MC->Scale(1/hist_MC->Integral());
    hist_rs->Scale(1/hist_rs->Integral());

    // Labels
    hist_MC->GetXaxis()->SetTitle("Reconstructed B^{0} mass / GeV");
    hist_MC->GetYaxis()->SetTitle("Normalized entries");

    // Colours
    hist_MC->SetLineColor(0);
    hist_MC->SetMarkerStyle(kPlus);
    hist_MC->SetMarkerColor(kRed);
    hist_rs->SetLineColor(0);
    hist_rs->SetMarkerStyle(kPlus);
    hist_rs->SetMarkerColor(kBlue);

    // Make legend
    TLegend * leg = new TLegend(0.55, 0.5, 0.85, 0.8);
    leg->AddEntry(hist_MC, "Fully simulated Monte Carlo");
    leg->AddEntry(hist_rs, "RapidSim");

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 800, 400);
    canvas->cd();
    hist_MC->Draw("P");
    hist_rs->Draw("P SAME");
    leg->Draw();

    // Save
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/Backgrounds/DKpipi/Plots/"
            "RS_vs_MC.pdf");

}
