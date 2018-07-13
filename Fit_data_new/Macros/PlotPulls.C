// Script to plot results and pulls for fit parameters
void PlotPulls(TString dir = "") {

    // =====
    // Setup
    // =====
    // Open the files
    TChain * toy_tree = new TChain("toy_tree");
    toy_tree->Add("../Results/FitterBias/" + dir + "/pulls_*.root");
    std::cout << "Loaded toy tree with " << toy_tree->GetEntries() << " entries."
        << std::endl;
    std::cout << "Entries with status = 0: " << toy_tree->GetEntries("status == 0") 
         << std::endl;
    std::cout << "Entries with covQual = 3: " << toy_tree->GetEntries("covQual == 3") 
         << std::endl;
    std::cout << "Total good toys: " << toy_tree->GetEntries("status == 0 && covQual == 3") 
         << std::endl;

    // Get list of parameters to loop through
    TFile * result_file = TFile::Open("../Results/twoAndFourBody_data.root", "READ");
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
    gStyle->SetOptStat(true);
    gStyle->SetStatX(0.85);

    // Make canvas
    TCanvas * canvas = new TCanvas("canvas", "", 1500, 400);

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
        // toy_tree->Draw(">>elist", "status == 0 && covQual == 3");
        toy_tree->Draw(">>elist", "covQual == 3");
        // toy_tree->Draw(">>elist");
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

        // std::cout << "Value max: " << value_max << std::endl;
        // std::cout << "Value min: " << value_min << std::endl;
        // std::cout << "Value buffer: " << value_buffer << std::endl;

        // Check limits of pulls
        double pull_max = 10;
        double pull_min = -1 * pull_max;
        int bins_pulls = n_bins;
        toy_tree->Draw(("signal_pull_" + par + ">>temp").c_str());
        TH1F * temp_hist = (TH1F*)gDirectory->Get("temp");
        // if (abs(mean) > pull_max) {
            // pull_max += abs(mean);
            // pull_min = -1 * pull_max;
            // std::cout << "Pull max: " << pull_max << std::endl;
            // bins_pulls = (int)(n_bins * (pull_max - pull_min)/10);
            // std::cout << "Bins for pulls: " << bins_pulls << std::endl;
        // }

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
        // toy_tree->Draw(("signal_final_value_" + par + ">>hist_value_" + par).c_str(),
                // " covQual == 3");
        toy_tree->Draw(("signal_final_value_" + par + ">>hist_value_bad_" + par).c_str(),
                "status != 0 || covQual != 3");
        toy_tree->Draw(("signal_final_error_" + par + ">>hist_error_" + par).c_str(), cut);
        toy_tree->Draw(("signal_pull_" + par + ">>hist_pulls_" + par).c_str(), cut);
        // toy_tree->Draw(("signal_final_error_" + par + ">>hist_error_" + par).c_str(),
                // " covQual == 3");
        // toy_tree->Draw(("signal_pull_" + par + ">>hist_pulls_" + par).c_str(),
                // " covQual == 3");
        canvas->Clear();

        // Plot values
        hist_value->SetLineWidth(1);
        hist_value->SetMarkerSize(0);
        hist_value->GetXaxis()->SetTitle((par + " value").c_str());
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
        hist_error->GetXaxis()->SetTitle((par + " error").c_str());
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

        // Draw line at fit starting value
        // TLine * init_line = new TLine(init_fit_vals[par], 0, init_fit_vals[par],
               // value_y_max);
        // init_line->SetLineColor(kGreen);
        // init_line->SetLineStyle(2);
        // init_line->Draw();
        // gPad->RedrawAxis();

        canvas->SaveAs((out_dir + par + ".pdf").c_str());
        canvas->Clear();

    } // End loop over parameters

}
