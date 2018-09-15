// Script to plot biases for toy fits
void PlotBias(TString dir = "") {

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
    TCanvas * canvas = new TCanvas("canvas", "", 500, 400);

    // Make histograms
    for (auto par : params_list) {

        // Remove "blind" from string
        size_t pos = par.find("_blind");
        par = par.substr(0, pos);

        // Get initial value 
        double init_value = toy_tree->GetMinimum(("signal_init_value_" + par).c_str());

        // Get minimum and maximum
        double bias_min = 0;
        double bias_max = 0;
        double value;
        double bias;
        toy_tree->SetBranchAddress(("signal_final_value_" + par).c_str(), &value);
        // toy_tree->Draw(">>elist", "status == 0 && covQual == 3");
        toy_tree->Draw(">>elist", "covQual == 3");
        TEventList * elist = (TEventList*)gDirectory->Get("elist");
        for (unsigned int i = 0; i < elist->GetN(); i++) {
            toy_tree->GetEntry(elist->GetEntry(i));
            bias = value - init_value;
            if (i == 0) {
                bias_min = bias;
                bias_max = bias;
            } else {
                if (bias < bias_min) bias_min = bias;
                if (bias > bias_max) bias_max = bias;
            }
        }
        
        // Get range of variable
        double bias_buffer = (bias_max - bias_min);

        // std::cout << "Value max: " << value_max << std::endl;
        // std::cout << "Value min: " << value_min << std::endl;
        // std::cout << "Value buffer: " << value_buffer << std::endl;

        // Make histogram
        TH1F * hist_value = new TH1F(("hist_value_" + par).c_str(), "", n_bins, 
                bias_min - bias_buffer, bias_max + bias_buffer);

        // Fill histogram
        for (unsigned int i = 0; i < elist->GetN(); i++) {
            toy_tree->GetEntry(elist->GetEntry(i));
            hist_value->Fill(value - init_value);
        }

        // Plot bias
        hist_value->SetLineWidth(1);
        hist_value->GetXaxis()->SetTitle("Bias");
        hist_value->GetYaxis()->SetRangeUser(0, hist_value->GetMaximum() * 1.6);
        canvas->cd();
        hist_value->Draw("E");

        // Fit the bias histogram with a Gaussian
        if (hist_value->Integral() != 0) {

            // Perform fit
            hist_value->Fit("gaus");
            TF1 * gauss_fit = hist_value->GetFunction("gaus");
            gauss_fit->SetLineColor(862);
            gauss_fit->SetLineWidth(1);

            // Draw
            gauss_fit->Draw("C SAME");
            hist_value->Draw("E SAME");

        } else {
            std::cout << "Could not fit bias for variable " << par <<
                std::endl;
        }

        // Save the canvas
        canvas->SaveAs((out_dir + par + "_bias.pdf").c_str());
        canvas->Clear();

    } // End loop over parameters

}
