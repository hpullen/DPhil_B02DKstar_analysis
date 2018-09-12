// ==============================================
// Script to compare rho MC between 2012 and 2016
// ==============================================
void compare_2012_2016() {

    // Open files: 2012
    TChain * chain_2012 = new TChain("DecayTree");
    chain_2012->Add("/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/"
            "2012_down/Kpi_selected.root");
    chain_2012->Add("/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/"
            "2012_up/Kpi_selected.root");

    // Open files: 2016
    TChain * chain_2016 = new TChain("DecayTree");
    chain_2016->Add("/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/"
            "2016_down/Kpi_selected.root");
    chain_2016->Add("/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/rho/"
            "2016_up/Kpi_selected.root");

    // Make histograms
    TH1F * hist_2012 = new TH1F("hist_2012", "", 50, 5000, 5800);
    TH1F * hist_2016 = new TH1F("hist_2016", "", 50, 5000, 5800);
    chain_2012->Draw("Bd_ConsD_M >> hist_2012");
    chain_2016->Draw("Bd_ConsD_M >> hist_2016");

    // Settings
    hist_2012->SetLineWidth(2);
    hist_2016->SetLineWidth(2);
    hist_2012->SetLineColor(kRed);
    hist_2016->SetLineColor(kBlue);

    // Normalise
    double max_2016 = hist_2016->GetMaximum();
    double max_2012 = hist_2012->GetMaximum();
    hist_2012->Scale(max_2016/max_2012);
    
    // Make legend
    TLegend leg(0.6, 0.7, 0.85, 0.85);
    leg.AddEntry(hist_2012, "2012", "L");
    leg.AddEntry(hist_2016, "2016", "L");

    // Draw
    TCanvas * canvas = new TCanvas ("canvas", "", 900, 600);
    hist_2012->Draw("C");
    hist_2016->Draw("C SAME");
    leg.Draw();
    gPad->RedrawAxis();
    canvas->SaveAs("2012_vs_2016.pdf");
}
