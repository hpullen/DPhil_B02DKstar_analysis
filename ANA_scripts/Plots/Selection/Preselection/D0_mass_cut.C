// ===================================================
// Script to produce a plot showing D0 mass cut region
// ===================================================
void D0_mass_cut() {

    // Open data: use 2016 mag down
    TFile * file = TFile::Open("/data/lhcb/users/pullen/B02DKstar/data/twoBody/"
            "2016_down/Full_dataset/Total.root", "READ");
    TTree * tree = (TTree*)file->Get("Tuple_Kpi/DecayTree");

    // Plot the D0 mass distribution
    double D0_mass = 1864.84;
    double min = D0_mass - 100;
    double max = D0_mass + 100;
    TH1F * mass_hist = new TH1F("mass_hist", "", 200, min, max);
    tree->Draw("D0_M>>mass_hist");

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    mass_hist->SetLineWidth(1);
    mass_hist->GetXaxis()->SetTitle("#it{m}(D^{0}) [MeV/#it{c}^{2}]");
    mass_hist->GetYaxis()->SetTitle("Candidates / (Mev/#it{c}^{2})");
    double y_max = mass_hist->GetMaximum() * 1.05;
    mass_hist->GetYaxis()->SetRangeUser(0, y_max);
    mass_hist->Draw("HIST");

    // Draw cut regions
    std::vector<TBox*> boxes;
    boxes.push_back(new TBox(D0_mass - 25, 0, D0_mass + 25, y_max));
    for (auto box : boxes) {
        box->SetFillColorAlpha(ANABlue, 0.3);
        box->Draw();
    }
    gPad->RedrawAxis();

    // Save
    canvas->cd();
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/ANA_resources/Plots/Selection/"
            "Preselection/D0_mass_cut.pdf");
}
