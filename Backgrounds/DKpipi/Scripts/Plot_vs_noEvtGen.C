// ===================================================
// Plot all components of DKpipi background normalized
// ===================================================
void Plot_vs_noEvtGen() {

    // List of modes to use
    std::map<TString, TString> modes = {
        {"2_Kstar", "2_Kstar"},
        {"3_K1_1400", "3_K1_1400"},
        // {"4_K0_1430", "4_K0_1430"},
        // {"6_K_rho", "6_K_rho"},
        // {"7a_K1_1270_Kstar", "7_K1_1270/Kstar"},
        // {"7b_K1_1270_rhoK", "7_K1_1270/rho_K"},
        // {"10_nonres", "10_nonres"}
    };


    // Open files
    std::map<TString, TTree *> trees;
    std::map<TString, TTree *> trees_old;
    TString dir = "/data/lhcb/users/pullen/RapidSim/B_DKpipi_study/";
    for (std::pair<TString, TString> mode : modes) {
        TFile * file = TFile::Open(dir + "with_evtGen/" + mode.first + "_tree.root", "READ");
        trees[mode.first] = (TTree*)file->Get("DecayTree");
        TFile * file_old = TFile::Open(dir + "Old_noEvtGen/" + mode.second + "/tree_full.root");
        trees_old[mode.first] = (TTree*)file_old->Get("DecayTree");
    }

    // Make cut
    TCut cut = "abs(Kstar_M - 0.8955) < 0.05 && abs(D0_M - 1.864) < 0.025";

    // Make histograms
    std::map<TString, TH1F*> histos;
    std::map<TString, TH1F*> histos_old;
    double min_Bd_M = 2.6;
    double max_Bd_M = 5.3;
    for (std::pair<TString, TString> mode : modes) {
        histos[mode.first] = new TH1F("hist_" + mode.first, "", 100, min_Bd_M, max_Bd_M);
        trees[mode.first]->Draw("Bd_M>>hist_" + mode.first, cut);
        histos[mode.first]->SetLineWidth(1);
        histos[mode.first]->SetMarkerStyle(0);
        histos[mode.first]->GetXaxis()->SetTitle("B^{0} mass (" + mode.first + ")");
        histos[mode.first]->GetYaxis()->SetTitle("Scaled entries");
        histos_old[mode.first] = new TH1F("hist_old_" + mode.first, "", 100, min_Bd_M, max_Bd_M);
        trees_old[mode.first]->Draw("Bd_M>>hist_old_" + mode.first, cut);
        histos_old[mode.first]->SetLineWidth(1);
        histos_old[mode.first]->Scale(histos[mode.first]->Integral()/
                histos_old[mode.first]->Integral());
        histos_old[mode.first]->SetLineColor(kRed);
        histos[mode.first]->SetMarkerStyle(0);
        histos_old[mode.first]->SetMarkerStyle(0);
    }


    // Make legend
    TLegend * leg = new TLegend(0.55, 0.75, 0.85, 0.9);
    leg->AddEntry(histos["2_Kstar"], "With EvtGen");
    leg->AddEntry(histos_old["2_Kstar"], "Without EvtGen");

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 800, 400);
    canvas->Divide(2, 1);
    int canv_num = 1;
    for (std::pair<TString, TString> mode : modes) {
        canvas->cd(canv_num);
        histos[mode.first]->Draw();
        histos_old[mode.first]->Draw("SAME");
        // leg->Draw();
        canv_num++;
    }

    // Save
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/Backgrounds/DKpipi/Plots/"
            "comparison_evtGen.pdf");

}
