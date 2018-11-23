// ===================================================
// Script to produce a plot showing D0 mass cut region
// ===================================================
void Kstar_mass_cut() {

    // Open data: use 2016 mag down
    TFile * file = TFile::Open("/data/lhcb/users/pullen/B02DKstar/data_preTISTOSbugfix/twoBody/"
            "2016_down/Full_dataset/Total.root", "READ");
    TTree * tree = (TTree*)file->Get("Tuple_Kpi/DecayTree");

    // Plot the Kstar mass distribution
    double Kstar_mass = 895.51;
    double min = 600;
    double max = 4100;
    TH1F * mass_hist = new TH1F("mass_hist", "", 450, min, max);
    tree->Draw("Kstar_M>>mass_hist");

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    mass_hist->SetLineWidth(1);
    mass_hist->GetXaxis()->SetTitle("#it{m}(K#pi) [MeV/#it{c}^{2}]");
    mass_hist->GetYaxis()->SetTitle("Candidates / (10 Mev/#it{c}^{2})");
    double y_max = mass_hist->GetMaximum() * 1.05;
    mass_hist->GetYaxis()->SetRangeUser(0, y_max);
    mass_hist->Draw("HIST");

    // Draw cut regions
    std::vector<TBox*> boxes;
    boxes.push_back(new TBox(min, 0, Kstar_mass - 50, y_max));
    boxes.push_back(new TBox(Kstar_mass + 50, 0, max, y_max));
    for (auto box : boxes) {
        box->SetFillColorAlpha(kRed, 0.2);
        box->Draw();
    }
    gPad->RedrawAxis();

    // Save
    canvas->cd();
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/ANA_resources/Plots/Selection/"
            "Preselection/Kstar_mass_cut.pdf");
}
