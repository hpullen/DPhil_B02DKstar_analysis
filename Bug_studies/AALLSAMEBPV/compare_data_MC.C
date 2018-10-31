// ===========================================
// Script to compare nPVs and MIPCHI2 in data and MC
// ===========================================
void compare_data_MC() {

    // Setup
    TString mode = "Kpi";
    std::vector<TString> vars = {
        "nPVs",
        "log10(D0K_MIPCHI2_PV)",
        "log10(D0Pi_MIPCHI2_PV)"
    };

    // Do for each Topo cut
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    TString bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") ?
        "fourBody" : "twoBody";
    for (auto var : vars) {
        TString year = "2016";

        // Get data
        TString dir = "/data/lhcb/users/pullen/B02DKstar/";
        TChain * tree_data = new TChain("DecayTree");
        tree_data->Add(dir + "/data/" + bod + "/" + year + "_down/" + mode + "_selected.root");
        tree_data->Add(dir + "/data/" + bod + "/" + year + "_up/" + mode + "_selected.root");

        // Get MC
        TChain * tree_MC = new TChain("DecayTree");
        tree_MC->Add(dir + "/MC/" + bod + "/" + mode + "/" + year + "_down/" + mode + "_selected.root");
        tree_MC->Add(dir + "/MC/" + bod + "/" + mode + "/" + year + "_up/" + mode + "_selected.root");

        // Loop through trigger lines
        for (TString topo : {"Topo2", "Topo3", "Topo4"}) {

            // Make histograms
            tree_data->Draw(var + ">>hist_data", "Bd_Hlt2" + topo + "BodyDecision_TOS && abs(Bd_ConsD_M - 5279.81) < 25");
            tree_MC->Draw(var + ">>hist_MC", "Bd_Hlt2" + topo + "BodyDecision_TOS");
            TH1F * hist_MC = (TH1F*)gDirectory->Get("hist_MC");
            TH1F * hist_data = (TH1F*)gDirectory->Get("hist_data");

            // Scale
            // hist_data->Scale(hist_MC->GetMaximum()/hist_data->GetMaximum());
            if (var == "nPVs") {
                hist_data->Scale(hist_MC->GetBinContent(2)/hist_data->GetBinContent(2));
            } else {
                hist_data->Scale(hist_MC->Integral()/hist_data->Integral());
            }

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
            TLegend leg(0.65, 0.7, 0.85, 0.9);
            leg.AddEntry(hist_data, "Data");
            leg.AddEntry(hist_MC, "Monte Carlo");
            leg.Draw("L");
            canvas->SaveAs("Plots/data_vs_MC_" + var + "_" + topo + ".pdf");
            delete hist_MC;
            delete hist_data;
        }
    }
}
