// ====================================================
// Plot Kstar invariant mass after selection is applied
// ====================================================
void Kstar_mass_after_selection() {

    // Get data
    TChain * tree = new TChain("DecayTree");
    for (TString year : {"2011", "2012", "2015", "2016"}) {
        for (TString mag : {"_down", "_up"}) {
            tree->Add("/data/lhcb/users/pullen/B02DKstar/data/twoBody/" 
                    + year + mag + "/Kpi_selected.root");
        }
    }

    // Draw
    TH1F * hist = new TH1F("hist", "", 100/2, 895.55 - 52, 895.55 + 52);
    tree->Draw("Kstar_M>>hist");
    hist->SetXTitle("#it{m}(K^{*0}) (MeV/#it{c}^{2})");
    hist->GetYaxis()->SetTitle("Candidates / 2 MeV/#it{c}^{2}");
    hist->SetLineWidth(1);
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    hist->Draw("E");
    canvas->SaveAs("../../../../ANA_resources/Plots/Selection/"
            "Preselection/Kstar_M_selected.pdf");
}
