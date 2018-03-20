// ===================================================
// Plot all components of DKpipi background normalized
// ===================================================
void PlotScaled() {

    // List of modes to use
    std::vector<TString> modes = {
        "2_Kstar",
        "3_K1_1400",
        "4_K0_1430",
        // "5a_K_1410_Kstar",
        // "5b_K_1410_rhoK",
        "6_K_rho",
        "7a_K1_1270_Kstar",
        "7b_K1_1270_rhoK",
        "10_nonres"
    };

    // Open files
    std::map<TString, TTree *> trees;
    TString dir = "/data/lhcb/users/pullen/RapidSim/B_DKpipi_study/trees_with_cuts/";
    for (TString mode : modes) {
        TFile * file = TFile::Open(dir + mode + "/" + mode + "_tree.root", "READ");
        trees[mode] = (TTree*)file->Get("DecayTree");
    }

    // Make cut
    TCut cut = "abs(Kstar_M - 0.8955) < 0.05 && Bd_M > 5";

    // Make histograms
    std::map<TString, TH1F*> histos;
    double max_Bd_M = 5.3;
    for (TString mode : modes) {
        histos[mode] = new TH1F("hist_" + mode, "", 100, 5, max_Bd_M);
        trees[mode]->Draw("Bd_M>>hist_" + mode, cut);
        histos[mode]->SetLineWidth(1);
        histos[mode]->GetXaxis()->SetTitle("Reconstructed B^{0} mass / GeV");
        histos[mode]->GetYaxis()->SetTitle("Scaled entries");
    }

    // Set colours
    histos["2_Kstar"]->SetLineColor(kBlue);
    histos["3_K1_1400"]->SetLineColor(kOrange);
    histos["4_K0_1430"]->SetLineColor(kYellow);
    // hist_5a->SetLineColor(kCyan);
    // hist_5b->SetLineStyle(2);
    // hist_5b->SetLineColor(kCyan);
    histos["6_K_rho"]->SetLineColor(kViolet);
    histos["7a_K1_1270_Kstar"]->SetLineColor(kMagenta);
    histos["7b_K1_1270_rhoK"]->SetLineStyle(2);
    histos["7b_K1_1270_rhoK"]->SetLineColor(kMagenta);
    histos["10_nonres"]->SetLineColor(kGreen);

    // Normalize histograms
    std::map<TString, double> scales = {
        {"2_Kstar", 0.175},
        {"3_K1_1400", 0.14},
        {"4_K0_1430", 0.13},
        {"5a_K_1410_Kstar", 0.12 * 0.9},
        {"5b_K_1410_rhoK", 0.12 * 0.1},
        {"6_K_rho", 0.11},
        {"7a_K1_1270_Kstar", 0.05 * 7 / 21},
        {"7b_K1_1270_rhoK", 0.05 * 14 / 21},
        {"10_nonres", 0.03}
    };
    // hist_1a->Scale(0.175 * 0.67 * 0.755 * 1/hist_1a->Integral());
    // hist_1b->Scale(0.175 * 0.33 * 0.104 * 1/hist_1b->Integral());
    for (TString mode : modes) {
        histos[mode]->Scale(scales[mode]);
    }

    // Make legend
    TLegend * leg = new TLegend(0.55, 0.2, 0.85, 0.9);
    // leg->AddEntry(hist_1a, "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            // "K^{*0}#pi^{+}) #bar{D}^{0}");
    // leg->AddEntry(hist_1b, "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            // "#rho^{0} K^{+}) #bar{D}^{0}");
    leg->AddEntry(histos["2_Kstar"], "B^{+} #rightarrow K^{*0} #bar{D}^{0} #pi^{+}");
    leg->AddEntry(histos["3_K1_1400"], "B^{+} #rightarrow (K_{1}(1400)^{+} "
            "#rightarrow K^{*0} #pi^{+}) #bar{D}^{0}");
    leg->AddEntry(histos["4_K0_1430"], "B^{+} #rightarrow K_{0}^{*}(1430)^{0} #pi^{+} "
            "#bar{D}^{0}");
    // leg->AddEntry(hist_5a, "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            // "K^{*0}#pi^{+}) #bar{D}^{0}");
    // leg->AddEntry(hist_5b, "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            // "#rho^{0} K^{+}) #bar{D}^{0}");
    leg->AddEntry(histos["6_K_rho"], "B^{+} #rightarrow K^{+} #rho^{0} #bar{D}^{0}");
    leg->AddEntry(histos["7a_K1_1270_Kstar"], "B^{+} #rightarrow (K_{1}(1270)^{+} "
            "#rightarrow K^{*0}#pi^{+}) #bar{D}^{0}");
    leg->AddEntry(histos["7b_K1_1270_rhoK"], "B^{+} #rightarrow (K_{1}(1270)^{+} "
            "#rightarrow #rho^{0} K^{+}) #bar{D}^{0}");
    leg->AddEntry(histos["10_nonres"], "B^{+} #rightarrow K^{+} #pi^{-} #pi^{+} "
            "#bar{D}^{0}");

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 800, 400);
    canvas->cd();
    histos["3_K1_1400"]->Draw();
    for (TString mode : modes) {
        histos[mode]->Draw("SAME");
    }
    leg->Draw();

    // // Check how much is covered by the 4 top components
    // double total = 0;
    // double top_4 = 0;
    // total += hist_1a->Integral();
    // total += hist_1b->Integral();
    // total += hist_2->Integral();
    // total += hist_3->Integral();
    // total += hist_4->Integral();
    // total += hist_5a->Integral();
    // total += hist_5b->Integral();
    // total += hist_6->Integral();
    // total += hist_7a->Integral();
    // total += hist_7b->Integral();
    // total += hist_10->Integral();
    // top_4 += hist_1a->Integral();
    // top_4 += hist_2->Integral();
    // top_4 += hist_4->Integral();
    // top_4 += hist_5a->Integral();
    // std::cout << "Top 4 cover " << top_4/total << " of the integral" << std::endl;

    // Save
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/Backgrounds/DKpipi/Plots/"
            "components_scaled.pdf");

}
