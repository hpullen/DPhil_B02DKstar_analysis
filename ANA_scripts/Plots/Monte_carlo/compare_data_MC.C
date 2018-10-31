// ===========================================
// Script to compare a variable in data and MC
// ===========================================
void compare_data_MC() {

    // Setup
    TString mode = "Kpi";
    std::vector<TString> vars = {
        "Bd_IPCHI2_OWNPV",
        "Bd_LOKI_VFASPF_VCHI2VDOF",
        "Bd_ptasy_1.50",
        "acos(Bd_DIRA_OWNPV)",
        "log10(D0_IPCHI2_OWNPV)",
        "log10(D0K_IPCHI2_OWNPV)",
        "log10(D0Pi_IPCHI2_OWNPV)",
        "log10(KstarK_IPCHI2_OWNPV)",
        "log10(KstarPi_IPCHI2_OWNPV)",
        "log10(D0K_PT)",
        "log10(D0Pi_PT)",
        "log10(KstarK_PT)",
        "log10(KstarPi_PT)"
    };
    std::map<TString, std::pair<double, double>> range  {
        {"Bd_IPCHI2_OWNPV", std::make_pair(0, 10)},
        {"Bd_LOKI_VFASPF_VCHI2VDOF", std::make_pair(0, 10)},
        {"Bd_ptasy_1.50", std::make_pair(-1, 1)},
        {"acos(Bd_DIRA_OWNPV)", std::make_pair(0, 0.015)},
        {"log10(D0_IPCHI2_OWNPV)", std::make_pair(0, 5)},
        {"log10(D0K_IPCHI2_OWNPV)", std::make_pair(0.5, 5)},
        {"log10(D0Pi_IPCHI2_OWNPV)", std::make_pair(0.5, 5)},
        {"log10(KstarK_IPCHI2_OWNPV)", std::make_pair(0.5, 5)},
        {"log10(KstarPi_IPCHI2_OWNPV)", std::make_pair(0, 5)},
        {"log10(D0K_PT)", std::make_pair(2, 5)},
        {"log10(D0Pi_PT)", std::make_pair(2, 5)},
        {"log10(KstarK_PT)", std::make_pair(2, 5)},
        {"log10(KstarPi_PT)", std::make_pair(2, 5)}
    };

    // Do for 2012 and 2016
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    TString bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") ?
        "fourBody" : "twoBody";
    for (auto var : vars) {
        for (TString year : {"2012", "2016"}) {

            // Get data
            TString dir = "/data/lhcb/users/pullen/B02DKstar/";
            TChain * tree_data = new TChain("DecayTree");
            tree_data->Add(dir + "/data/" + bod + "/" + year + "_down/" + mode + "_selected.root");
            tree_data->Add(dir + "/data/" + bod + "/" + year + "_up/" + mode + "_selected.root");

            // Get MC
            TChain * tree_MC = new TChain("DecayTree");
            tree_MC->Add(dir + "/MC/" + bod + "/" + mode + "/" + year + "_down/" + mode + "_selected.root");
            tree_MC->Add(dir + "/MC/" + bod + "/" + mode + "/" + year + "_up/" + mode + "_selected.root");

            // Make histograms
            int n_bins = 40;
            TH1F * hist_MC = new TH1F("hist_MC", "", n_bins, range[var].first, range[var].second);
            TH1F * hist_data = new TH1F("hist_data", "", n_bins, range[var].first, range[var].second);
            tree_data->Draw(var + ">>hist_data", "abs(Bd_ConsD_M - 5279.81) < 25");
            tree_MC->Draw(var + ">>hist_MC");

            // Scale
            // hist_data->Scale(hist_MC->GetMaximum()/hist_data->GetMaximum());
            hist_data->Scale(hist_MC->Integral()/hist_data->Integral());

            // Colours and style
            hist_MC->SetLineColor(kBlack);
            hist_MC->SetLineWidth(1);
            hist_data->SetLineWidth(1);
            hist_data->SetLineColor(kRed);
            hist_data->SetMarkerColor(kRed);
            hist_data->SetMarkerStyle(1);

            // Draw and save
            canvas->Clear();
            hist_data->SetXTitle(var);
            hist_MC->SetXTitle(var);
            if (hist_data->GetMaximum() > hist_MC->GetMaximum()) {
                hist_data->Draw("E");
            } else {
                hist_MC->Draw("HIST");
            }
            hist_MC->Draw("HIST SAME");
            hist_data->Draw("E SAME");
            canvas->SaveAs("../../../ANA_resources/Plots/Monte_carlo/data_vs_MC/"
                    + var + "_" + year + ".pdf");
            delete hist_MC;
            delete hist_data;
        }
    }
}
