// Script for fitting and plotting a significance distribution
void PlotSignificance(TString mode) {

    // =====
    // Setup
    // =====
    // Open the files
    TChain * tree = new TChain("toy_tree");
    tree->Add("/data/lhcb/users/pullen/B02DKstar/toys/significance/toys/" + mode + "/*.root");
    std::cout << "Loaded tree with " << tree->GetEntries() << " entries."
        << std::endl;
    // std::cout << "Entries with status = 0: " << tree->GetEntries("status == 0");

    // Make and fill histogram
    double max = (mode == "pipipipi") ? 20 : 10;
    TH1F * hist = new TH1F("Significance", "#piK significance", 100, 0, max);
    hist->SetStats(true);
    // tree->Draw("significance>>Significance", "status == 0");
    tree->Draw("significance>>Significance");

    // Fit
    hist->Fit("gaus");
    TF1 * gauss_fit = hist->GetFunction("gaus");
    gauss_fit->SetLineColor(862);
    gauss_fit->SetLineWidth(1);

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 500, 400);
    hist->SetLineWidth(1);
    hist->GetXaxis()->SetTitle("Significance");
    hist->GetYaxis()->SetTitle("Number of toys");
    gStyle->SetOptStat(true);
    gStyle->SetStatX(0.85);
    hist->Draw("E");
    gauss_fit->SetLineWidth(2);
    gauss_fit->Draw("C SAME");
    canvas->SaveAs("../Plots/Significance/significance_" + mode + ".pdf");
}
