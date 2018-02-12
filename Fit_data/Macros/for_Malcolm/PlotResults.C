// Script to plot results, pulls and significance for results files in a given directory
void PlotResults(std::string dir) {

    // Open the files
    TChain * toy_tree = new TChain("toy_tree");
    toy_tree->Add((dir + "/" + "results_*.root").c_str());
    std::cout << "Loaded toy tree with " << toy_tree->GetEntries() << " entries."
        << std::endl;

    // Check which parameters will be contained in the tree
    bool free_Kpi_yield = false;
    bool free_expo_yield = false;
    if (dir.find("free_Kpi_free_expo") != std::string::npos) {
        free_Kpi_yield = true; 
    } 
    if (dir.find("free_expo") != std::string::npos) {
        free_expo_yield = true;
    }

    // Name of directories to save to
    TString dirname; 
    if (free_Kpi_yield && free_expo_yield) dirname = "free_Kpi_free_expo";
    else if (free_Kpi_yield && !free_expo_yield) dirname = "free_Kpi_fixed_expo";
    else if (!free_Kpi_yield && free_expo_yield) dirname = "fixed_Kpi_free_expo";

    // List of parameters to loop through
    std::vector<TString> params_list = {"R_piK_vs_Kpi"};
    if (free_Kpi_yield) params_list.push_back("n_signal_Kpi");
    if (free_expo_yield) params_list.push_back("n_expo");

    // Make histograms
    // Map to hold histograms
    std::map<TString, TH1F*> hist_map;
    int n_bins = 50;

    // Kpi signal yield
    if (free_Kpi_yield) {
        hist_map.emplace("value_n_signal_Kpi", new TH1F("hist_value_n_signal_Kpi", "", n_bins, 600, 1000));
        hist_map.emplace("null_value_n_signal_Kpi", new TH1F("hist_null_value_n_signal_Kpi", "", n_bins, 600, 1000));
        hist_map.emplace("error_n_signal_Kpi", new TH1F("hist_error_n_signal_Kpi", "", n_bins, 25, 35));
        hist_map.emplace("null_error_n_signal_Kpi", new TH1F("hist_null_error_n_signal_Kpi", "", n_bins, 25, 35));
        hist_map.emplace("pull_null_n_signal_Kpi", new TH1F("hist_pull_null_n_signal_Kpi", "", n_bins, -2, 2));
    }

    // piK exponential background yield
    if (free_expo_yield) {
        hist_map.emplace("value_n_expo", new TH1F("hist_value_n_expo", "", n_bins, 800, 1200));
        hist_map.emplace("null_value_n_expo", new TH1F("hist_null_value_n_expo", "", n_bins, 800, 1200));
        hist_map.emplace("error_n_expo", new TH1F("hist_error_n_expo", "", n_bins, 25, 40));
        hist_map.emplace("null_error_n_expo", new TH1F("hist_null_error_n_expo", "", n_bins, 25, 40));
        hist_map.emplace("pull_null_n_expo", new TH1F("hist_pull_null_n_expo", "", n_bins, -2, 5));
    }

    // Ratio between piK and Kpi
    hist_map.emplace("value_R_piK_vs_Kpi", new TH1F("hist_value_R_piK_vs_Kpi", "", n_bins, 0, 0.15));
    hist_map.emplace("null_value_R_piK_vs_Kpi", new TH1F("hist_null_value_R_piK_vs_Kpi", "", n_bins, 0, 0.15));
    hist_map.emplace("error_R_piK_vs_Kpi", new TH1F("hist_error_R_piK_vs_Kpi", "", n_bins, 0, 0.05));
    hist_map.emplace("null_error_R_piK_vs_Kpi", new TH1F("hist_null_error_R_piK_vs_Kpi", "", n_bins, 0, 0.05));
    hist_map.emplace("pull_null_R_piK_vs_Kpi", new TH1F("hist_pull_null_R_piK_vs_Kpi", "", n_bins, -10, 5));

    // Significance
    TH1F * hist_sig = new TH1F("hist_significance", "", 20, 0, 10);

    // Loop through parameters and plot the results
    for (auto param : params_list) {

        // Draw into histograms
        toy_tree->Draw("final_value_" + param + ">>hist_value_" + param, "status == 0");
        toy_tree->Draw("null_value_" + param + ">>hist_null_value_" + param, "status == 0");
        toy_tree->Draw("error_" + param + ">>hist_error_" + param, "status == 0");
        toy_tree->Draw("null_error_" + param + ">>hist_null_error_" + param, "status == 0");
        toy_tree->Draw("pull_null_" + param + ">>hist_pull_null_" + param, "status == 0");
        toy_tree->Draw("pull_init_vs_final_" + param + ">>hist_pull_init_vs_final_" + param, "status == 0");

        // Make canvas
        TCanvas * canvas = new TCanvas("canvas_" + param, "", 1500, 400);
        canvas->Divide(3, 1);

        // Plot values
        hist_map["value_" + param]->SetLineColor(kRed);
        hist_map["value_" + param]->SetLineWidth(1);
        hist_map["value_" + param]->GetXaxis()->SetTitle(param + " value");
        hist_map["null_value_" + param]->SetLineColor(862);
        hist_map["null_value_" + param]->SetLineWidth(1);
        canvas->cd(1);
        hist_map["value_" + param]->Draw();
        hist_map["null_value_" + param]->Draw("SAME");

        // Make legend
        TLegend * leg = new TLegend(0.55, 0.7, 0.85, 0.9);
        leg->AddEntry(hist_map["value_" + param], "Normal fit");
        leg->AddEntry(hist_map["null_value_" + param], "Null hypothesis");
        leg->Draw();

        // Plot errors
        hist_map["error_" + param]->SetLineColor(kRed);
        hist_map["error_" + param]->SetLineWidth(1);
        hist_map["error_" + param]->GetXaxis()->SetTitle("Error");
        hist_map["null_error_" + param]->SetLineColor(862);
        hist_map["null_error_" + param]->SetLineWidth(1);
        canvas->cd(2);
        hist_map["error_" + param]->Draw();
        hist_map["null_error_" + param]->Draw("SAME");
        leg->Draw();

        // Plot pulls
        hist_map["pull_null_" + param]->SetLineWidth(1);
        hist_map["pull_null_" + param]->GetXaxis()->SetTitle("Pull");
        hist_map["pull_null_" + param]->GetYaxis()->SetRangeUser(0, 
                hist_map["pull_null_" + param]->GetMaximum() * 1.6);
        canvas->cd(3);
        hist_map["pull_null_" + param]->Draw("E");

        // Fit the pull histogram with a Gaussian
        if (hist_map["pull_null_" + param]->Integral() != 0) {

            // Perform fit
            hist_map["pull_null_" + param]->Fit("gaus");
            TF1 * gauss_fit = hist_map["pull_null_" + param]->GetFunction("gaus");
            gauss_fit->SetLineColor(862);
            gauss_fit->SetLineWidth(1);

            // Draw
            gauss_fit->Draw("C SAME");
            hist_map["pull_null_" + param]->Draw("E SAME");

        } else {
            std::cout << "Could not fit pull for variable " << param <<
                std::endl;
        }

        // Save the canvas
        canvas->SaveAs("Plots/" + dirname + "/pulls_" + param + ".pdf");
        delete canvas;

    } // End loop over parameters

    // Fit and plot significance
    TCanvas * sig_canv = new TCanvas("sig_canv", "", 500, 400);
    toy_tree->Draw("significance>>hist_significance", "status == 0");
    if (hist_sig->Integral() != 0) {

        // Perform fit
        hist_sig->Fit("gaus");
        TF1 * gauss_fit = hist_sig->GetFunction("gaus");
        gauss_fit->SetLineColor(kBlue);
        gauss_fit->SetLineWidth(2);

        // Plot
        hist_sig->SetLineWidth(1);
        hist_sig->SetMarkerStyle(2);
        hist_sig->SetStats(kTRUE);
        hist_sig->Draw("P");
        gauss_fit->Draw("C SAME");
        hist_sig->Draw("P SAME");
        sig_canv->SaveAs("Plots/" + dirname + "/significance.pdf");

    } else {
        std::cout << "Error: could not fit significance!" << std::endl;
    }

}
