// ==========================================
// Plot significance as a function of BDT cut
// ==========================================
void PlotSignificance(TString mode) {

    // File location
    TString dir = "/data/lhcb/users/pullen/B02DKstar/toys/significance"
        "/toys/BDT/" + mode;

    // TGraph for results
    TGraphErrors * graph = new TGraphErrors();

    // Loop through cuts
    std::vector<TString> cuts = {"-0.5", "-0.4", "-0.3", "-0.2", 
        "-0.1", "0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6",
        "0.7", "0.8", "0.9"};
    int count = 0;
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    double err_x = 0.05;
    for (auto cut : cuts) {

        // Open files
        TChain * tree = new TChain("toy_tree");
        tree->Add(dir + "/" + cut + "/*.root");

        // Draw significance
        TString hist_name = "hist_" + cut;
        tree->Draw("significance>>" + hist_name);
        TH1F * hist = (TH1F*)gDirectory->Get(hist_name);

        // Fit
        hist->Fit("gaus");
        TF1 * fit = hist->GetFunction("gaus");

        // Plot histogram and fit
        canvas->Clear();
        hist->Draw("E");
        fit->Draw("C SAME");
        canvas->SaveAs("Plots/Significance/Fits/" + mode + "/fit_" 
                + cut + ".pdf");

        // Fill graph
        graph->SetPoint(count, std::stod(std::string(cut)), fit->GetParameter("Mean"));
        graph->SetPointError(count, err_x, fit->GetParameter("Sigma"));

        // Increment datapoint count
        count++;

    }

    // Draw and save graph
    canvas->Clear();
    graph->GetXaxis()->SetTitle(mode + " BDT cut");
    graph->GetYaxis()->SetTitle("Significance");
    graph->Draw("AP");
    canvas->SaveAs("Plots/Significance/" + mode + ".pdf");  
}
