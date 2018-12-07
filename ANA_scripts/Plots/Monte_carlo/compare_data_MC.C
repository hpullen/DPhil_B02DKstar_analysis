// ===========================================
// Script to compare a variable in data and MC
// ===========================================
void compare_data_MC(TString weight = "") {

    // Setup
    for (TString mode : {"Kpi", "Kpipipi"}) {
        std::vector<TString> vars = {
            "Bd_IPCHI2_OWNPV",
            "Bd_LOKI_VFASPF_VCHI2VDOF",
            "Bd_ptasy_1.50",
            "acos(Bd_DIRA_OWNPV)",
            "log10(D0_IPCHI2_OWNPV)",
            "log10(KstarK_IPCHI2_OWNPV)",
            "log10(KstarPi_IPCHI2_OWNPV)",
            "log10(KstarK_PT)",
            "log10(KstarPi_PT)",
            "log10(Bd_PT)",
            "Bd_ETA"
        };
        if (mode == "Kpi") {
            vars.push_back("log10(D0K_IPCHI2_OWNPV)");
            vars.push_back("log10(D0Pi_IPCHI2_OWNPV)");
            vars.push_back("log10(D0K_PT)");
            vars.push_back("log10(D0Pi_PT)");
            vars.push_back("D0Pi_ETA");
            vars.push_back("D0K_ETA");
            vars.push_back("KstarK_ETA");
            vars.push_back("KstarPi_ETA");
        }
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
            {"log10(KstarPi_PT)", std::make_pair(2, 5)},
            {"log10(Bd_PT)", std::make_pair(2, 5)},
            {"log10(Bd_ETA)", std::make_pair(0, 5)},
            {"log10(D0K_ETA)", std::make_pair(0, 5)},
            {"log10(D0Pi_ETA)", std::make_pair(0, 5)},
            {"log10(KstarK_ETA)", std::make_pair(0, 5)},
            {"log10(KstarPi_ETA)", std::make_pair(0, 5)}
        };

        // Do for 2012 and 2016
        TCanvas * canvas = new TCanvas("canvas", "", 500, 400);
        TString bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") ?
            "fourBody" : "twoBody";
        for (auto var : vars) {
            for (TString year : {"2012", "2016"}) {

                // Change variable name if needed
                TString mc_var = var;
                std::string var_str = std::string(var);
                if (year == "2012" && var_str.find("_ETA") != std::string::npos) {
                    mc_var = (var_str.substr(0, var_str.find("_ETA")) + "_LOKI_ETA").c_str();
                }

                // Get data
                TString dir = "/data/lhcb/users/pullen/B02DKstar/";
                TChain * tree_data = new TChain("DecayTree");
                tree_data->Add(dir + "/data/" + bod + "/" + year + "_down/" + mode + "_selected_withWeights.root");
                tree_data->Add(dir + "/data/" + bod + "/" + year + "_up/" + mode + "_selected_withWeights.root");

                // Get MC
                TChain * tree_MC = new TChain("DecayTree");
                tree_MC->Add(dir + "/MC/" + bod + "/" + mode + "/" + year + "_down/" + mode + "_selected.root");
                tree_MC->Add(dir + "/MC/" + bod + "/" + mode + "/" + year + "_up/" + mode + "_selected.root");

                // Make histograms
                int n_bins = 20;
                TH1F * hist_MC = new TH1F("hist_MC", "", n_bins, range[var].first, range[var].second);
                TH1F * hist_MC_fine = new TH1F("fine_hist_MC", "", n_bins * 1000, range[var].first, range[var].second);
                TH1F * hist_data = new TH1F("hist_data", "", n_bins, range[var].first, range[var].second);
                TH1F * hist_data_fine = new TH1F("fine_hist_data", "", n_bins * 1000, range[var].first, range[var].second);
                TString data_weight = "sw_signal";
                TString mc_weight = (weight == "") ? "1" : weight;
                tree_data->Draw(var + ">>hist_data", data_weight);
                tree_data->Draw(var + ">>fine_hist_data", data_weight);
                tree_MC->Draw(mc_var + ">>hist_MC", mc_weight);
                tree_MC->Draw(mc_var + ">>fine_hist_MC", mc_weight);

                // Scale
                hist_data->Scale(hist_MC->Integral()/hist_data->Integral());

                // Colours and style
                hist_MC->SetLineColor(kBlack);
                hist_MC->SetLineWidth(1);
                hist_data->SetLineWidth(1);
                hist_data->SetLineColor(kRed);
                hist_data->SetMarkerColor(kRed);
                hist_data->SetMarkerStyle(1);

                // Draw 
                canvas->Clear();
                hist_data->SetXTitle(var);
                hist_MC->SetXTitle(var);
                if (hist_data->GetMaximum() * 1.2 > hist_MC->GetMaximum()) {
                    hist_data->GetYaxis()->SetRangeUser(0, hist_data->GetMaximum() * 1.2);
                    hist_data->Draw("E");
                } else {
                    hist_MC->Draw("HIST");
                }
                hist_MC->Draw("HIST SAME");
                hist_data->Draw("E SAME");

                // Apply test and make box
                double ks = hist_data_fine->KolmogorovTest(hist_MC_fine);
                TPaveText * ks_text = new TPaveText(0.7, 0.85, 0.85, 0.9, "NDC");
                ks_text->SetLineColor(kBlack);
                ks_text->SetLineWidth(kBlack);
                ks_text->SetFillColor(1);
                ks_text->SetFillStyle(0);
                ks_text->SetShadowColor(0);
                ks_text->SetCornerRadius(0);
                ks_text->SetBorderSize(1);
                std::stringstream ss;
                ss << "KS: " << std::fixed << std::setprecision(4) << ks;
                ks_text->AddText(ss.str().c_str());
                ks_text->Draw();
                canvas->Modified();
                canvas->Update();

                // Save
                TString save_name = var;
                if (var == "Bd_ptasy_1.50") {
                    save_name = "Bd_ptasy_1_50";
                }
                canvas->SaveAs("../../../ANA_resources/Plots/Monte_carlo/data_vs_MC/"
                        + weight + "/" + mode + "/" + save_name + "_" + year + ".pdf");
                delete hist_MC;
                delete hist_data;
                delete hist_MC_fine;
                delete hist_data_fine;
            }
        }
    }
}
