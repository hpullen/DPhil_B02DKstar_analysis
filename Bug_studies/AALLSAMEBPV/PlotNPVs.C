// ========================================================
// Plot number of PVs for different trigger lines and years
// ========================================================
void PlotNPVs(TString mode = "Kpi") {

    // Draw nPVs for different Topo decisions
    // Get data
    TChain * tree = new TChain("DecayTree");
    TString dir = "/data/lhcb/users/pullen/B02DKstar/";
    tree->Add(dir + "data/twoBody/2016_down/Kpi_selected.root");
    tree->Add(dir + "data/twoBody/2016_up/Kpi_selected.root");

    for (TString topo : {"2", "3", "4"}) {

        // Plot nPVs with and without topo cut
        tree->Draw("nPVs>>hist_PVs");
        tree->Draw("nPVs>>hist_PVs_topo" + topo, "Bd_Hlt2Topo" + topo + "BodyDecision_TOS");

        // Draw histograms together
        TH1F * hist_orig = (TH1F*)gDirectory->Get("hist_PVs");
        TH1F * hist_4 = (TH1F*)gDirectory->Get("hist_PVs_topo" + topo);
        hist_orig->SetLineColor(kBlack);
        hist_orig->SetMarkerColor(kBlack);
        hist_4->SetLineColor(ANABlue);
        hist_4->SetMarkerColor(ANABlue);
        TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
        double entries_orig = hist_orig->GetBinContent(2);
        double entries_4 = hist_4->GetBinContent(2);
        hist_4->Scale(entries_orig/entries_4);
        hist_4->SetXTitle("nPVs");
        hist_4->Draw("E");
        hist_orig->Draw("E SAME");

        // Make legend and save
        TLegend leg(0.55, 0.65, 0.85, 0.9);
        leg.AddEntry(hist_orig, "All cuts");
        leg.AddEntry(hist_4, "Hlt2Topo" + topo + " line");
        leg.Draw("L");
        canvas->SaveAs("Plots/nPVs_topo" + topo + ".pdf");
    }

    // Draw MIPCHI2 for D0 daughter
    for (TString topo : {"2", "3", "4"}) {

        // Plot nPVs with and without topo cut
        tree->Draw("log(D0K_MIPCHI2_PV)>>hist_MIPCHI2");
        tree->Draw("log(D0K_MIPCHI2_PV)>>hist_MIPCHI2_topo" + topo, "Bd_Hlt2Topo" + topo + "BodyDecision_TOS");

        // Draw histograms together
        TH1F * hist_orig = (TH1F*)gDirectory->Get("hist_MIPCHI2");
        TH1F * hist_4 = (TH1F*)gDirectory->Get("hist_MIPCHI2_topo" + topo);
        hist_orig->SetLineColor(kBlack);
        hist_orig->SetMarkerColor(kBlack);
        hist_4->SetLineColor(ANABlue);
        hist_4->SetMarkerColor(ANABlue);
        TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
        hist_4->Scale(hist_orig->Integral()/hist_4->Integral());
        hist_4->SetXTitle("log(D^{0}K #chi_{IP}^{2})");
        hist_4->Draw("E");
        hist_orig->Draw("E SAME");

        // Make legend and save
        TLegend leg(0.55, 0.7, 0.85, 0.9);
        leg.AddEntry(hist_orig, "All selected data");
        leg.AddEntry(hist_4, "Data passing Hlt2Topo" + topo + " line");
        leg.Draw("L");
        canvas->SaveAs("Plots/MIPCHI2_topo" + topo + ".pdf");
    }


}
