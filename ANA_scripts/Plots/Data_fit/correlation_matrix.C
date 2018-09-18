// ================================
// Plot data fit correlation matrix
// ================================
// Get names of certain observables
TString get_name(TString par) {

    // Remove blind
    std::string par_long = std::string(par);
    std::string par_short = par_long.substr(0, par_long.find("_blind"));

    // Map of names 
    std::map<std::string, std::string> names = {
        {"A_signal_Kpi", "A^{K#pi}"},
        {"R_signal_piK_plus", "R_{#pi K}^{+}"},
        {"R_signal_piK_minus", "R_{#pi K}^{-}"},
        {"A_Bs_piK", "A_{s}^{#pi K}"},
        {"A_signal_Kpipipi", "A^{K#pi#pi#pi}"},
        {"R_signal_piKpipi_plus", "R_{#pi K#pi#pi}^{+}"},
        {"R_signal_piKpipi_minus", "R_{#pi K#pi#pi}^{-}"},
        {"A_Bs_piKpipi", "A_{s}^{#pi K#pi#pi}"},
        {"R_signal_pipipipi_run2", "R^{4#pi}"},
        {"A_signal_pipipipi_run2", "A^{4#pi}"},
        {"R_ds_pipipipi_run2", "R_{ds}^{4#pi}"},
        {"N_Bs_pipipipi_run2", "N_{Bs}^{4#pi}"},
        {"A_Bs_pipipipi_run2", "A_{s}^{4#pi}"}
    };

    // Add GLW variables
    for (std::string run : {"1", "2"}) {
        names["A_signal_KK_run" + run] = "A^{KK," + run + "}";
        names["R_signal_KK_run" + run] = "R^{K," + run + "}";
        names["R_ds_KK_run" + run] = "R_{ds}^{KK," + run + "}";
        names["N_Bs_KK_run" + run] = "N_{Bs}^{KK," + run + "}";
        names["A_Bs_KK_run" + run] = "A_{s}^{KK" + run + "}";
        names["A_signal_pipi_run" + run] = "A^{#pi#pi " + run + "}";
        names["R_signal_pipi_run" + run] = "R^{#pi#pi " + run + "}";
        names["R_ds_pipi_run" + run] = "R_{ds}^{#pi#pi " + run + "}";
        names["N_Bs_pipi_run" + run] = "N_{Bs}^{#pi#pi," + run + "}";
        names["A_Bs_pipi_run" + run] = "A_{s}^{#pi#pi " + run + "}";
    }

    // Search and return
    if (names.find(par_short) != names.end()) {
        return names[par_short].c_str();
    } else {
        return par_short;
    }
}


void correlation_matrix() {

    TCanvas * canvas = new TCanvas("canvas", "", 12000, 6000);
    gROOT->ForceStyle();

    // Get fit result
    TFile * file = TFile::Open("../../../Fit_data/Results/twoAndFourBody_data_split.root", "READ");
    RooFitResult * r = (RooFitResult*)file->Get("fit_result");

    // Parameters of interest
    std::vector<TString> pars = {
        "A_signal_Kpi",
        "R_signal_piK_plus_blind",
        "R_signal_piK_minus_blind",
        "A_Bs_piK",
        "A_signal_Kpipipi",
        "R_signal_piKpipi_plus_blind",
        "R_signal_piKpipi_minus_blind",
        "A_Bs_piKpipi"
    };
    for (TString run : {"_run1", "_run2"}) {
        for (TString mode : {"KK", "pipi", "pipipipi"}) {
            if (mode == "pipipipi" && run == "_run1") continue;
            pars.push_back("A_signal_" + mode + run + "_blind");
            pars.push_back("R_signal_" + mode + run + "_blind");
            // pars.push_back("R_ds_" + mode + run + "_blind");
            pars.push_back("N_Bs_" + mode + run);
            pars.push_back("A_Bs_" + mode + run);
        }
    }

    // Make histogram
    int N = pars.size();
    std::cout << "N: " << N << std::endl;
    TH2D * hist = new TH2D("correlation", "", N, 0, N, N, 0, N);

    // Loop through and fill
    int i = 0;
    for (auto par1 : pars) {
        int j = 0;
        for (auto par2 : pars) {
            // Get correlation
            double corr = r->correlation("pdf_params_" + par1, "pdf_params_" + par2);
            std::stringstream ss;
            if (corr >= 0.0001) {
                ss << std::setprecision(3) << corr * 100;
            } else {
                ss << "0.000";
            }
            hist->Fill(i, j, std::stod(ss.str()));
            hist->GetYaxis()->SetBinLabel(j + 1, get_name(par2));
            j++;
        }
        hist->GetXaxis()->SetBinLabel(i + 1, get_name(par1));
        i++;
    }

    // Draw and save
    canvas->cd();
    // canvas->SetLeftMargin(0.3);
    // canvas->SetBottomMargin(0.25);
    hist->GetXaxis()->SetLabelSize(0.03);
    hist->GetXaxis()->SetLabelOffset(0.004);
    hist->GetYaxis()->SetLabelOffset(0.004);
    hist->GetYaxis()->SetLabelSize(0.03);
    hist->SetMarkerSize(0.75);
    hist->SetMarkerColor(kBlack);
    hist->GetZaxis()->SetRangeUser(-100, 100);
    hist->Draw("COLZ TEXT");
    canvas->SaveAs("../../../ANA_resources/Plots/Data_fit/correlation.pdf");


}
