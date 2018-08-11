// ==========================================================
// Plot uncertainty vs. BDT cut for a particular D decay mode
// ==========================================================
void PlotUncertainties(TString mode) {

    // Get parameters of interest
    std::vector<TString> params = {};
    if (mode == "piK") {
        params = {"R_signal_piK_plus", "R_signal_piK_minus"};
    } else if (mode == "piKpipi") {
        params = {"R_signal_piKpipi_plus", "R_signal_piKpipi_minus"};
    } else if (mode == "KK") {
        params = {"R_signal_KK_run1", "R_signal_KK_run2", "A_signal_KK_run1", 
            "A_signal_KK_run2"};
    } else if (mode == "pipi") {
        params = {"R_signal_pipi_run1", "R_signal_pipi_run2", "A_signal_pipi_run1", 
            "A_signal_pipi_run2"};
    }

    // Upper limits for plotting uncertainty
    std::map<TString, double> upper_lim = {
        {"R_signal_piK_minus", 0.05},
        {"R_signal_piK_plus", 0.05},
        {"R_signal_piKpipi_minus", 0.05},
        {"R_signal_piKpipi_plus", 0.05},
        {"R_signal_KK_run1", 0.3},
        {"R_signal_KK_run2", 0.3}
    };

    // Location of files
    TString dir = "/data/lhcb/users/pullen/B02DKstar/BDT_studies/" + mode;

    // Make TGraphs for results
    std::map<TString, TGraph*> graphs;
    for (auto param : params) {
        graphs[param] = new TGraph();
    }

    // Loop through cuts
    std::vector<std::string> cuts = {"0.1", "0.2", "0.3", "0.4", "0.5", "0.6",
        "0.7", "0.8", "0.9"};
    int count = 0;
    TCut qual_cut = "status == 0 && covQual == 3";
    for (auto cut : cuts) {

        // Open files for the cut
        TChain * tree = new TChain("toy_tree");
        tree->Add(dir + "/" + cut + "/*.root");
        std::cout << tree->GetEntries(qual_cut) << " for cut " << cut << std::endl;

        // Loop through parameters
        for (auto param : params) {

            // Ignore bad datapoints
            int count_to_use = count;
            if (param == "A_signal_KK_run1" && cut == "0.1") continue;
            if (param == "A_signal_KK_run1") count_to_use = count - 1;
            if (mode == "KK" && cut == "0.9") continue;

            // Full name
            TString par = "fit_shape_final_error_" + param;

            // Check for upper limit
            TCut upper_cut = "";
            auto it = upper_lim.find(param);
            if (it != upper_lim.end()) {
                upper_cut = par + " < " + std::to_string(it->second).c_str();
            }

            // Draw histogram
            TString hist_name = "hist_" + cut + "_" + param;
            tree->Draw(par + ">>" + hist_name, qual_cut + upper_cut);
            TH1F * hist = (TH1F*)gDirectory->Get(hist_name);

            // Fit to uncertainty
            hist->Fit("gaus");
            TF1 * fit = hist->GetFunction("gaus");
            double error = fit->GetParameter("Mean");

            // Add to graph
            double cut_val = std::stod(cut);
            graphs[param]->SetPoint(count_to_use, cut_val, error);

            // Save plot of histogram and fit
            TCanvas * canv = new TCanvas("canv", "", 900, 600);
            hist->SetLineWidth(1);
            hist->Draw("E");
            fit->Draw("C SAME");
            hist->Draw("E SAME");
            canv->SaveAs("Plots/Fits/" + mode + "_" + param + "_" + cut + ".pdf");
        }

        // Increment graph point count
        count++;
    }

    // Draw each graph
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    for (auto graph : graphs) {

        // Setup 
        graph.second->GetXaxis()->SetTitle(mode + " BDT cut");
        graph.second->GetYaxis()->SetTitle(graph.first + " uncertainty");

        // Draw and save
        canvas->Clear();
        graph.second->Draw();
        canvas->SaveAs("Plots/" + mode + "_" + graph.first + ".pdf");
    }
}
