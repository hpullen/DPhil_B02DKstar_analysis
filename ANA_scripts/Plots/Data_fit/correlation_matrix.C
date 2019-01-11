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
        {"A_Bs_piKpipi", "A_{s}^{#pi K#pi#pi}"}
    };

    // Add GLW variables
    names["A_signal_KK"] = "A^{KK}";
    names["R_signal_KK"] = "R^{K}";
    names["R_ds_KK"] = "R_{ds}^{KK}";
    names["N_Bs_KK"] = "N_{Bs}^{KK}";
    names["A_Bs_KK"] = "A_{s}^{KK}";
    names["A_signal_pipi"] = "A^{#pi#pi}";
    names["R_signal_pipi"] = "R^{#pi#pi}";
    names["R_ds_pipi"] = "R_{ds}^{#pi#pi}";
    names["N_Bs_pipi"] = "N_{Bs}^{#pi#pi}";
    names["A_Bs_pipi"] = "A_{s}^{#pi#pi}";

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
    TFile * file = TFile::Open("../../../Fit_data/Results/twoAndFourBody_data_split_combinedRuns.root", "READ");
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
    for (TString mode : {"KK", "pipi"}) {
        pars.push_back("A_signal_" + mode + "_blind");
        pars.push_back("R_signal_" + mode + "_blind");
        pars.push_back("N_Bs_" + mode);
        pars.push_back("A_Bs_" + mode);
    }

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
    hist->SetTickLength(0.01);
    hist->GetXaxis()->SetTickSize(0.01);
    hist->GetYaxis()->SetTickSize(0.01);
    hist->GetZaxis()->SetRangeUser(-100, 100);
    hist->Draw("COLZ TEXT");
    canvas->Update();
    TPaletteAxis * pal = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject("palette");
    pal->GetAxis()->SetLabelSize(0.03);
    hist->Draw("COLZ TEXT");
    canvas->SaveAs("../../../ANA_resources/Plots/Data_fit/correlation.pdf");


}
