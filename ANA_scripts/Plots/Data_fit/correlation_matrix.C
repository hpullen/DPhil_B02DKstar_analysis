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
        "R_signal_piKpipi_plus_blind",
        "R_signal_piKpipi_minus_blind",
        "A_Bs_piK",
        "A_signal_Kpipipi",
        "R_signal_piKpipi_plus_blind",
        "R_signal_piKpipi_minus_blind",
        "A_Bs_piKpipi"
    };

    // Make histogram
    int N = pars.size();
    std::cout << "N: " << N << std::endl;
    TH2D * hist = new TH2D("correlation", "", N, 0, 100, N, 0, 100);

    // Loop through and fill
    int i = 1;
    for (auto par1 : pars) {
        int j = 1;
        for (auto par2 : pars) {
            // Get correlation
            double corr = r->correlation("pdf_params_" + par1, "pdf_params_" + par2);
            std::cout << i << ", " << j << ": correlation = " << corr << std::endl;
            hist->Fill(i, j, corr * 100);
            hist->GetYaxis()->SetBinLabel(j, get_name(par2));
            j++;
        }
        hist->GetXaxis()->SetBinLabel(i, get_name(par1));
        i++;
    }
    hist->Fill(8, 8, 1000);

    // Draw and save
    canvas->cd();
    canvas->SetLeftMargin(0.3);
    canvas->SetBottomMargin(0.25);
    hist->SetMarkerSize(2);
    hist->SetMarkerColor(kWhite);
    hist->Draw("COLZ TEXT");
    canvas->SaveAs("../../../ANA_resources/Plots/Data_fit/correlation.pdf");


}
