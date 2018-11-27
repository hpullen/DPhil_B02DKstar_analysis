// =============================================
// Plot B0 mass distribution after preselection 
// =============================================
void B0_mass() {

    // Open data files
    TString dir = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    TChain * tree = new TChain("DecayTree");
    for (TString year : {"2016", "2015"}) {
        for (TString mag : {"_up", "_down"}) {
            tree->Add(dir + year + mag + "/Kpi.root");
        }
    }

    // Draw B mass
    TH1F * hist = new TH1F("hist", "", 200, 4700, 6200);
    tree->Draw("Bd_ConsD_M>>hist");
    int bin_width = hist->GetBinWidth(1);

    // Set maximum
    double y_max = hist->GetMaximum() * 1.05;
    hist->GetYaxis()->SetRangeUser(0, y_max);

    // Draw hist
    hist->SetLineWidth(1);
    hist->SetLineColor(kBlack);
    hist->GetXaxis()->SetTitle("#it{m}([K#pi]_{D}K^{*0}) [MeV/#it{c}^{2}]");
    hist->GetYaxis()->SetTitle(("Candidates / (" + std::to_string(bin_width) + 
                " MeV/#it{c}^{2})").c_str());
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    hist->Draw("HIST");

    // Add line at 5800
    TLine line(5800, 0, 5800, y_max);
    line.SetLineStyle(2);
    line.SetLineColor(kRed);
    line.Draw();
    gPad->RedrawAxis();

    // Save
    canvas->SaveAs("../../../../ANA_resources/Plots/Selection/Preselection/B0_mass.pdf");
}
