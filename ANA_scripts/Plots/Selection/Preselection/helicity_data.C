// =====================================================
// Script to plot helicity angle in MC and selected data
// =====================================================
void helicity_data() {

    // Open MC and data
    gROOT->ForceStyle();
    TChain * mc_tree = new TChain("DecayTree");
    TChain * data_tree = new TChain("DecayTree");
    std::vector<TString> years = {"2015_down", "2015_up", 
        "2016_down", "2016_up"};
    for (auto year : years) {
        mc_tree->Add("/data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/" + year +
                "/Full_helicity/Kpi.root");
        data_tree->Add("/data/lhcb/users/pullen/B02DKstar/data/twoBody/" +
                year + "/Kpi_selected_withWeights.root");
    }

    // Get normalisation weight for MC
    double entries_mc_inRange = mc_tree->GetEntries("abs(Kstar_helicity_angle) > 0.4");
    double entries_mc = mc_tree->GetEntries();
    double weight = entries_mc_inRange/entries_mc;

    // Draw
    TH1F * hist_mc = new TH1F("hist_mc", "", 50, -1, 1);
    TH1F * hist_data = new TH1F("hist_data", "", 50, -1, 1);
    mc_tree->Draw("Kstar_helicity_angle>>hist_mc", "Bd_BKGCAT == 0");
    data_tree->Draw("Kstar_helicity_angle>>hist_data");

    // Plot
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    hist_mc->SetLineColor(kRed);
    hist_mc->Scale(hist_data->Integral()/(hist_mc->Integral() * weight));
    hist_mc->GetXaxis()->SetTitle("cos(#theta)");
    hist_mc->GetYaxis()->SetTitle("Candidates");
    hist_mc->SetLineWidth(1);
    hist_mc->Draw("HIST");
    hist_data->SetLineColor(kBlack);
    hist_data->SetMarkerColor(kBlack);
    hist_data->SetLineWidth(1);
    hist_data->Draw("E SAME");

    // Draw legend
    TLegend * leg = new TLegend(0.2, 0.75, 0.5, 0.9);
    leg->AddEntry(hist_mc, "Monte Carlo", "L");
    leg->AddEntry(hist_data, "sWeighted data");
    leg->SetBorderSize(1);
    leg->Draw();
    
    // Save 
    canvas->SaveAs("../../../../ANA_resources/Plots/Selection/Preselection/"
            "helicity_angle_data.pdf");
}
    
