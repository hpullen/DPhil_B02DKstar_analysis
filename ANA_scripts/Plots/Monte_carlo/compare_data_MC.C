// ===========================================
// Script to compare a variable in data and MC
// ===========================================
void compare_data_MC(TString var, TString mode = "Kpi") {

    // Do for 2012 and 2016
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    TString bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") ?
        "fourBody" : "twoBody";
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
        tree_data->Draw(var + ">>hist_data_" + var + year, 
                "abs(Bd_ConsD_M - 5279.81) < 25");
        tree_data->Draw(var + ">>hist_MC_" + var + year);
        TH1F * hist_data = (TH1F*)gDirectory->Get("hist_data_" + var + year);
        TH1F * hist_MC = (TH1F*)gDirectory->Get("hist_MC_" + var + year);

        // Scale
        hist_MC->Scale(1/hist_MC->Integral());
        hist_data->Scale(1/hist_data->Integral());

        // Colours and style
        hist_MC->SetLineColor(kBlack);
        hist_data->SetLineColor(kRed);
        hist_data->SetMarkerColor(kRed);

        // Draw and save
        canvas->Clear();
        hist_MC->Draw("HIST");
        hist_data->Draw("E SAME");
        canvas->SaveAs("../../../ANA_resources/Plots/Monte_carlo/data_vs_MC/"
                + var + "_" + year + ".root");
    }
}
