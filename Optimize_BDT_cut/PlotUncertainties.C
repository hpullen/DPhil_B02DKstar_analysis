// ==========================================================
// Plot uncertainty vs. BDT cut for a particular D decay mode
// ==========================================================
void PlotUncertainties(TString mode) {

    // Get parameters of interest
    std::vector<TString> params = {};
    if (mode == "Kpi") {
        params = {"R_signal_piK_plus", "R_signal_piK_minus"};
    } else if (mode == "Kpipipi") {
        params = {"R_signal_piKpipi_plus", "R_signal_piKpipi_minus"};
    } else if (mode == "KK") {
        params = {"R_signal_KK_run1", "R_signal_KK_run2", "A_signal_KK_run1", 
            "A_signal_KK_run2"};
    } else if (mode == "pipi") {
        params = {"R_signal_pipi_run1", "R_signal_pipi_run2", "A_signal_pipi_run1", 
            "A_signal_pipi_run2"};
    } else if (mode == "pipipipi") {
        params = {"R_signal_pipipipi_run2", "A_signal_pipipipi_run2"};
    }

    // Upper limits for plotting value
    std::map<TString, double> err_upper_lim = {
        {"R_signal_piK_minus", 0.03},
        {"R_signal_piK_plus", 0.03}
        // {"R_signal_piKpipi_minus", 0.2},
        // {"R_signal_piKpipi_plus", 0.2},
        // {"R_signal_KK_run1", 1.5},
        // {"R_signal_KK_run2", 1.5}
    };

    // Previous analysis uncertainties
    double run2_scale = 1/sqrt(1.5);
    double both_scale = 1/sqrt(2.5);
    std::map<TString, double> prev_errs = {
        {"A_signal_KK_run1", 0.145},
        {"R_signal_KK_run1", 0.165},
        {"A_signal_pipi_run1", 0.217},
        {"R_signal_pipi_run1", 0.283},
        {"A_signal_KK_run2", 0.145 * run2_scale},
        {"R_signal_KK_run2", 0.165 * run2_scale},
        {"A_signal_pipi_run2", 0.217 * run2_scale},
        {"R_signal_pipi_run2", 0.283 * run2_scale},
        {"R_signal_piK_plus", 0.029 * both_scale},
        {"R_signal_piK_minus", 0.032 * both_scale}
    };

    // Location of files
    TString dir = "/data/lhcb/users/pullen/B02DKstar/BDT_studies/toys/" + mode;

    // Make TGraphs for results
    std::map<TString, std::pair<TGraphErrors*, TGraphErrors*>> graphs;
    for (auto param : params) {
        graphs[param].first = new TGraphErrors();
        graphs[param].second = new TGraphErrors();
    }

    // TGraph for stability plot
    TGraphErrors * stability = new TGraphErrors();

    // Loop through cuts
    std::vector<std::string> cuts;
    if (mode == "Kpi" || mode == "Kpipipi") {
        cuts = {"-0.2", "-0.1", "0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", 
            "0.7", "0.8", "0.9"};
    } else {
        cuts = {"0.0", "0.1", "0.2", "0.3", "0.40", "0.45", "0.50", "0.55", "0.60", 
            "0.65", "0.70", "0.75", "0.80", "0.85", "0.90", "0.95"};
    }
    int count = 0;
    TCut qual_cut = "status == 0 && covQual == 3";
    TCanvas * canv = new TCanvas("canv", "", 900, 600);
    for (auto cut : cuts) {

        // Open files for the cut
        TChain * tree = new TChain("toy_tree");
        tree->Add(dir + "/" + cut + "/*.root");

        // Get stability
        std::cout << tree->GetEntries(qual_cut) << " for cut " << cut << std::endl;
        double stab_val = (double)tree->GetEntries(qual_cut)/(double)tree->GetEntries() * 100;
        std::cout << "Stability value: " << stab_val << std::endl;
        double err_x = (mode == "Kpi" || mode == "Kpipipi") ? 0.05 : 0.025;
        stability->SetPoint(count, std::stod(cut), stab_val);
        stability->SetPointError(count, err_x, 0);

        // Loop through parameters
        for (auto param : params) {

            // Ignore bad datapoints
            int count_to_use = count;
            if (param == "A_signal_KK_run1" && cut == "0.1") continue;
            if (param == "A_signal_KK_run1") count_to_use = count - 1;
            if (mode == "KK" && cut == "0.9") continue;

            // Full name
            // TString par = "fit_shape_final_value_" + param;
            TString par = "fit_shape_final_value_" + param;
            TString err = "fit_shape_final_error_" + param;

            // Check for upper limit
            TCut upper_cut = "";
            auto it = err_upper_lim.find(param);
            if (it != err_upper_lim.end()) {
                upper_cut = err + " < " + std::to_string(it->second).c_str();
            }

            // Draw histograms: value and uncertainty
            TString hist_name = "hist_" + cut + "_" + param;
            TH1F * hist = new TH1F(hist_name, "", 50, 0, 0);
            tree->Draw(par + ">>" + hist_name, qual_cut);
            // TH1F * hist = (TH1F*)gDirectory->Get(hist_name);
            // TString hist_err_name = "hist_err_" + cut + "_" + param;
            // tree->Draw(err + ">>" + hist_err_name, qual_cut + upper_cut);
            // TH1F * hist_err = (TH1F*)gDirectory->Get(hist_err_name);

            // Fit to value
            hist->Fit("gaus");
            TF1 * fit = hist->GetFunction("gaus");
            double sigma = fit->GetParameter("Sigma");
            double sigma_err = fit->GetParError(fit->GetParNumber("Sigma"));

            // Add to value width graph
            double cut_val = std::stod(cut);
            graphs[param].first->SetPoint(count_to_use, cut_val, sigma);
            graphs[param].first->SetPointError(count_to_use, err_x, sigma_err);

            // // Fit to uncertainty
            // hist_err->Fit("gaus");
            // TF1 * fit_err = hist_err->GetFunction("gaus");
            // double mean = fit_err->GetParameter("Mean");
            // double mean_err = fit->GetParError(fit->GetParNumber("Mean"));

            // // Add to error graph
            // graphs[param].second->SetPoint(count_to_use, cut_val, mean);
            // graphs[param].second->SetPointError(count_to_use, err_x, mean_err);

            // Save plot of histogram and fit
            canv->Clear();
            hist->SetLineWidth(1);
            hist->Draw("E");
            fit->Draw("C SAME");
            hist->Draw("E SAME");
            canv->SaveAs("Plots/Fits/" + mode + "/" + param + "_" + cut + ".pdf");

            // // Save plot of error histogram and fit
            // canv->Clear();
            // hist_err->SetLineWidth(1);
            // hist_err->Draw("E");
            // fit_err->Draw("C SAME");
            // hist_err->Draw("E SAME");
            // canv->SaveAs("Plots/Fits/" + mode + "/" + param + "_err_" + cut + ".pdf");
        }
        

        // Increment graph point count
        count++;
    }

    // Draw each graph
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    for (auto graph : graphs) {

        // Setup 
        graph.second.first->GetXaxis()->SetTitle(mode + " BDT cut");
        graph.second.first->GetYaxis()->SetTitle(graph.first + " uncertainty");

        // Draw and save
        canvas->Clear();
        graph.second.first->Draw("AP");

        // Make line for previous analysis value
        TLine line(graph.second.first->GetXaxis()->GetXmin(), 0, graph.second.first->GetXaxis()->GetXmax(), 0);
        line.SetLineStyle(2);
        line.SetLineColor(kRed);
        if (prev_errs.find(graph.first) != prev_errs.end()) {
            line.SetY1(prev_errs[graph.first]);
            line.SetY2(prev_errs[graph.first]);
            canvas->cd();
            line.Draw();
            // graph.second.first->Draw("AP SAME");
        }
        canvas->SaveAs("Plots/" + mode + "_" + graph.first + "_fromValue.pdf");

        // // Setup
        // graph.second.second->GetXaxis()->SetTitle(mode + " BDT cut");
        // graph.second.second->GetYaxis()->SetTitle(graph.first + " uncertainty");

        // // Draw and save
        // canvas->Clear();
        // graph.second.second->Draw("AP");
        // line.Draw();
        // canvas->SaveAs("Plots/" + mode + "_" + graph.first + "_fromError.pdf");
    }

    // Draw stability graph
    stability->GetXaxis()->SetTitle(mode + " BDT cut");
    stability->GetYaxis()->SetTitle("Fit stability (%)");
    // stability->GetYaxis()->SetRangeUser(0, 100);
    canvas->Clear();
    stability->Draw("AP");
    canvas->SaveAs("Plots/" + mode + "_stability.pdf");
}
