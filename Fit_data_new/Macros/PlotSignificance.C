// Script for fitting and plotting a significance distribution
void PlotSignificance() {

    // =====
    // Setup
    // =====
    // Open the files
    TChain * tree = new TChain("toy_tree");
    tree->Add("../Results/Significance/significance_piK_*.root");
    std::cout << "Loaded tree with " << tree->GetEntries() << " entries."
        << std::endl;
    std::cout << "Entries with status = 0: " << tree->GetEntries("status == 0");

    // Make and fill histogram
    TH1F * hist = new TH1F("hist_sig", "", 100, 0, 10);
    hist->SetStats(true);
    tree->Draw("significance>>hist_sig", "status == 0");

    // // Fit
    // hist->Fit("gaus");
    // TF1 * gauss_fit = hist->GetFunction("gaus");
    // gauss_fit->SetLineColor(862);
    // gauss_fit->SetLineWidth(1);

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 500, 400);
    // gauss_fit->Draw("C SAME");
    hist->Draw("E SAME");
    canvas->SaveAs("../Plots/Significance/significance.pdf");
}
