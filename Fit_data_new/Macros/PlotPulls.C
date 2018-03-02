// Script to plot results and pulls for fit parameters
void PlotPulls() {

    // =====
    // Setup
    // =====
    // Open the files
    TChain * toy_tree = new TChain("toy_tree");
    toy_tree->Add("../Results/FitterBias/pulls_*.root");
    std::cout << "Loaded toy tree with " << toy_tree->GetEntries() << " entries."
        << std::endl;

    // Get list of parameters to loop through
    TFile * result_file = TFile::Open("../Results/twoAndFourBody_data.root", "READ");
    RooFitResult * result = (RooFitResult*)result_file->Get("fit_result");
    RooArgList vars = result->floatParsFinal();
    RooRealVar * var;
    std::vector<std::string> params_list;
    TIterator * it = vars.createIterator();
    while ((var = (RooRealVar*)it->Next())) {
        std::string fullname = var->GetName();
        std::string prefix = "pdf_params_";
        params_list.push_back(fullname.substr(prefix.length(), std::string::npos));
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
        
        // Get range of variable
        double value_min = toy_tree->GetMinimum(("signal_final_value_" + par).c_str());
        double value_max = toy_tree->GetMaximum(("signal_final_value_" + par).c_str());
        double value_buffer = (value_max - value_min)/8;
        double error_min = toy_tree->GetMinimum(("signal_error_" + par).c_str());
        double error_max = toy_tree->GetMaximum(("signal_error_" + par).c_str());
        double error_buffer = (error_max - error_min)/8;

        // Make histograms: value, error, pull
        TH1F * hist_value = new TH1F(("hist_value_" + par).c_str(), "", n_bins, 
                value_min - value_buffer, value_max + value_buffer);
        TH1F * hist_error = new TH1F(("hist_error_" + par).c_str(), "", n_bins,
                error_min - error_buffer, error_max + error_buffer);
        TH1F * hist_pulls = new TH1F(("hist_pulls_" + par).c_str(), "", n_bins, -10, 10);

        // Fill histograms
        toy_tree->Draw(("signal_final_value_" + par + ">>hist_value_" + par).c_str(),
                "status == 0");
        toy_tree->Draw(("signal_error_" + par + ">>hist_error_" + par).c_str(),
                "status == 0");
        toy_tree->Draw(("signal_pull_" + par + ">>hist_pulls_" + par).c_str(),
                "status == 0");
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

        // Make legend
        TLegend * leg = new TLegend(0.55, 0.7, 0.85, 0.9);
        leg->AddEntry(hist_value, "Final value");
        leg->AddEntry(value_line, "Initial value");
        leg->Draw();

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
        canvas->SaveAs(("../Plots/FitterBias/" + par + ".pdf").c_str());
        canvas->Clear();

    } // End loop over parameters

}
