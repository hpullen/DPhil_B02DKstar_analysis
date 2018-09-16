// Script to plot results and pulls for fit parameters
// Get names of certain observables
std::string get_name(std::string par) {

    // Map of names 
    std::map<std::string, std::string> names = {
        {"A_signal_Kpi", "A^{K#pi}"},
        {"Rstd::string", "R_{#pi K}^+"},
        {"R_signal_piK_minus", "R_{#pi K}^-"},
        {"A_Bs_piK", "A_{s}^{#pi K}"},
        {"A_signal_Kpipipi", "A^{K#pi#pi#pi}"},
        {"R_signal_piKpipi_plus", "R_{#pi K#pi#pi}^+"},
        {"R_signal_piKpipi_minus", "R_{#pi K#pi#pi}^-"},
        {"A_Bs_piKpipi", "A_{s}^{#pi K#pi#pi}"},
        {"R_signal_pipipipi_run2", "R^{4#pi}"},
        {"A_signal_pipipipi_run2", "A^{4#pi}"},
        {"R_ds_pipipipi_run2", "R_{ds}^{4#pi}"},
        {"A_Bs_pipipipi_run2", "A_{s}^{4#pi}"}
    };

    // Add GLW variables
    for (std::string run : {"1", "2"}) {
        names["A_signal_KK_run" + run] = "A^{KK," + run + "}";
        names["R_signal_KK_run" + run] = "A^{#pi#pi," + run + "}";
        names["R_ds_KK_run" + run] = "R_{ds}^{KK," + run + "}";
        names["A_Bs_KK_run" + run] = "A_{s}^{KK" + run + "}";
        names["A_signal_pipi_run" + run] = "A^{#pi#pi " + run + "}";
        names["R_signal_pipi_run" + run] = "R^{#pi#pi " + run + "}";
        names["R_ds_pipi_run" + run] = "R_{ds}^{#pi#pi " + run + "}";
        names["A_Bs_pipi_run" + run] = "A_{s}^{#pi#pi " + run + "}";
    }

    // Search and return
    if (names.find(par) != names.end()) {
        return names[par];
    } else {
        return par;
    }
}


// Main script
void PlotPulls(TString dir = "") {

    // =====
    // Setup
    // =====
    // Open the files
    TChain * toy_tree = new TChain("toy_tree");
    toy_tree->Add("../Results/FitterBias/" + dir + "/pulls_*.root");
    std::cout << "Loaded toy tree with " << toy_tree->GetEntries() << " entries." << std::endl;
    std::cout << "Entries with status = 0: " << toy_tree->GetEntries("status == 0") 
         << std::endl;
    std::cout << "Entries with covQual = 3: " << toy_tree->GetEntries("covQual == 3") 
         << std::endl;
    int good_entries = toy_tree->GetEntries("status == 0 && covQual == 3");
    std::cout << "Total good toys: " << good_entries << std::endl;

    // Get list of parameters to loop through
    bool split = true;
    TString results_filename = split ? "../Results/twoAndFourBody_data_split.root" :
        "../Results/twoAndFourBody_data.root";
    TFile * result_file = TFile::Open(results_filename, "READ");
    RooFitResult * result = (RooFitResult*)result_file->Get("fit_result");
    RooArgList vars = result->floatParsInit();
    RooRealVar * var;
    std::vector<std::string> params_list;
    TIterator * it = vars.createIterator();
    std::map<std::string, double> init_fit_vals;
    while ((var = (RooRealVar*)it->Next())) {
        std::string fullname = var->GetName();
        std::string prefix = "pdf_params_";
        std::string shortname = fullname.substr(prefix.length(), std::string::npos);
        params_list.push_back(shortname);
        init_fit_vals.emplace(shortname, var->getVal());
    }

    // Output directory for histograms
    std::string out_dir = "../Plots/FitterBias/";
    if (dir.Length() != 0) {
        out_dir = "../Results/FitterBias/" + dir + "/";
    }


    // ===============
    // Make histograms
    // ===============
    // Map to hold histograms
    std::map<TString, TH1F*> hist_map;
    int n_bins = 50;

    // Stats box options
    // gStyle->SetOptStat(true);
    // gStyle->SetOptFit(1);
    // gStyle->SetStatX(0.85);

    // Make canvas
    TCanvas * canvas = new TCanvas("canvas", "", 1500, 400);

    // Open output file
    std::fstream bias_file("../Results/biases.param");

    // Make histograms
    for (auto par : params_list) {

        // Remove "blind" from string
        size_t pos = par.find("_blind");
        par = par.substr(0, pos);

        // Get minimum and maximum
        double value_min = 0;
        double value_max = 0;
        double error_min = 0;
        double error_max = 0;
        double value;
        double error;
        toy_tree->SetBranchAddress(("signal_final_value_" + par).c_str(), &value);
        toy_tree->SetBranchAddress(("signal_final_error_" + par).c_str(), &error);
        toy_tree->Draw(">>elist", "status == 0 && covQual == 3");
        TEventList * elist = (TEventList*)gDirectory->Get("elist");
        for (unsigned int i = 0; i < elist->GetN(); i++) {
            toy_tree->GetEntry(elist->GetEntry(i));
            if (i == 0) {
                value_min = value;
                value_max = value;
                error_min = error;
                error_max = error;
            } else {
                if (value < value_min) value_min = value;
                if (value > value_max) value_max = value;
                if (error < error_min) error_min = error;
                if (error > error_max) error_max = error;
            }
        }
        
        // Get range of variable
        double value_buffer = (value_max - value_min);
        double error_buffer = (error_max - error_min);

        // Check limits of pulls
        double pull_max = 10;
        double pull_min = -1 * pull_max;
        int bins_pulls = n_bins;
        toy_tree->Draw(("signal_pull_" + par + ">>temp").c_str());
        TH1F * temp_hist = (TH1F*)gDirectory->Get("temp");

        // Make histograms: value, error, pull
        TH1F * hist_value = new TH1F(("hist_value_" + par).c_str(), "", n_bins, 
                value_min - value_buffer, value_max + value_buffer);
        TH1F * hist_value_bad = new TH1F(("hist_value_bad_" + par).c_str(), "", n_bins, 
                value_min - value_buffer, value_max + value_buffer);
        TH1F * hist_error = new TH1F(("hist_error_" + par).c_str(), "", n_bins,
                error_min - error_buffer, error_max + error_buffer);
        TH1F * hist_pulls = new TH1F(("hist_pulls_" + par).c_str(), "", bins_pulls, 
               pull_min, pull_max);

        // Fill histograms
        TCut cut = "status == 0 && covQual == 3";
        toy_tree->Draw(("signal_final_value_" + par + ">>hist_value_" + par).c_str(), cut);
        toy_tree->Draw(("signal_final_value_" + par + ">>hist_value_bad_" + par).c_str(),
                "status != 0 || covQual != 3");
        toy_tree->Draw(("signal_final_error_" + par + ">>hist_error_" + par).c_str(), cut);
        toy_tree->Draw(("signal_pull_" + par + ">>hist_pulls_" + par).c_str(), cut);
        canvas->Clear();

        // Plot values
        hist_value->SetLineWidth(1);
        hist_value->SetMarkerSize(0);
        hist_value->GetXaxis()->SetTitle((get_name(par) + " value").c_str());
        hist_value->GetXaxis()->SetTitleOffset(1.1);
        hist_value->SetStats(false);
        canvas->Divide(3, 1);
        canvas->cd(1);
        hist_value->Draw();
        canvas->Update();

        // Draw line at initial value 
        double init_value = toy_tree->GetMinimum(("signal_init_value_" + par).c_str());
        double value_y_max = canvas->GetPad(1)->GetUymax();
        TLine * value_line = new TLine(init_value, 0, init_value, value_y_max);
        value_line->SetLineColor(kRed);
        value_line->SetLineStyle(2);
        value_line->Draw();
        gPad->RedrawAxis();

        // Plot errors
        hist_error->SetLineWidth(1);
        hist_error->GetXaxis()->SetTitle((get_name(par) + " error").c_str());
        hist_error->GetXaxis()->SetTitleOffset(1.1);
        hist_error->SetStats(false);
        canvas->cd(2);
        hist_error->Draw();
        canvas->Update();

        // Draw line at initial error
        double init_error = toy_tree->GetMinimum(("signal_init_error_" + par).c_str());
        double error_y_max = gPad->GetUymax();
        TLine * error_line = new TLine(init_error, 0, init_error, error_y_max);
        error_line->SetLineColor(kRed);
        error_line->SetLineStyle(2);
        error_line->Draw();
        gPad->RedrawAxis();

        // Plot pulls
        hist_pulls->SetLineWidth(1);
        hist_pulls->GetXaxis()->SetTitle("Pull");
        hist_pulls->GetXaxis()->SetTitleOffset(1.1);
        hist_pulls->GetYaxis()->SetRangeUser(0, hist_pulls->GetMaximum() * 1.6);
        canvas->cd(3);
        hist_pulls->Draw("E");

        // Fit the pull histogram with a Gaussian
        if (hist_pulls->Integral() != 0) {

            // Perform fit
            hist_pulls->Fit("gaus");
            TF1 * gauss_fit = hist_pulls->GetFunction("gaus");
            gauss_fit->SetLineColor(862);
            gauss_fit->SetLineWidth(1);

            // Draw
            gauss_fit->Draw("C SAME");
            hist_pulls->Draw("E SAME");

            // Make stats box
            TPaveText * stats = new TPaveText(0.55, 0.7, 0.85, 0.9, "NDC");
            stats->SetLineColor(0);
            stats->SetFillColor(0);
            stats->SetShadowColor(0);
            stats->SetCornerRadius(0);
            stats->SetBorderSize(0);
            std::stringstream mu_stream;
            mu_stream << "#mu = ";
            mu_stream << std::setprecision(3) << gauss_fit->GetParameter("Mean"); 
            mu_stream << " #pm " << std::setprecision(2) << gauss_fit->GetParError(gauss_fit->GetParNumber("Mean"));
            stats->AddText(mu_stream.str().c_str());
            std::stringstream sigma_stream;
            sigma_stream << "#sigma = ";
            sigma_stream << std::setprecision(4) << gauss_fit->GetParameter("Sigma"); 
            sigma_stream << " #pm " << std::setprecision(2) << gauss_fit->GetParError(gauss_fit->GetParNumber("Sigma"));
            stats->AddText(sigma_stream.str().c_str());
            TVirtualPad * pad = canvas->cd(3);
            stats->Draw();
            pad->Modified();
            pad->Update();

            // Write to file
            bias_file << par << " " << gauss_fit->GetParameter("Mean")
                << " " << gauus_fit->GetParError(gauss_fit->GetParNumber("Mean"))
                << std::endl;

        } else {
            std::cout << "Could not fit pull for variable " << par <<
                std::endl;
        }

        // Save the canvas
        canvas->SaveAs((out_dir + par + ".pdf").c_str());

        // Draw on bad values
        hist_value_bad->SetLineWidth(1);
        hist_value_bad->SetLineColor(kBlack);
        hist_value_bad->SetFillColorAlpha(kRed, 0.5);
        canvas->cd(1);
        hist_value_bad->Draw("HIST SAME");

        // Save canvas
        canvas->SaveAs((out_dir + par + "_withFails.pdf").c_str());
        canvas->Clear();
        bias_file.close();

    } // End loop over parameters

}
