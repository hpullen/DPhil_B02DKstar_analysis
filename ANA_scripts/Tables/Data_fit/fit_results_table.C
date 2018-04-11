// ==============================
// Function to estimate precision
// ==============================
std::pair<int, int> getPrecision(double value, double error) {

    // Get order of magnitude of value and uncertainty
    int scale_value = (int)log10(std::abs(value));
    if (value < 1) scale_value -= 1;
    int scale_error = (int)log10(std::abs(error));
    if (error < 1) scale_error -= 1;

    // Calculate precisions
    int prec_value = scale_value - scale_error + 1;
    int prec_error = 1;

    // Adjust if first digit of error is 1
    double digit_error = error / pow(10, scale_error);
    if (digit_error < 2) {
        prec_value += 1;
        prec_error +=1;
    }

    // Return 
    return std::make_pair(prec_value, prec_error);

}

// ============================================================
// Script to make table of floating parameters from fit to data
// ============================================================
void fit_results_table() {

    // Open RooFitResult
    TFile * result_file = TFile::Open("/home/pullen/analysis/B02DKstar/"
            "Fit_data_new/Results/twoAndFourBody_data_split.root", "READ");
    RooFitResult * r = (RooFitResult*)result_file->Get("fit_result");

    // Open table files
    std::map<std::string, std::ofstream*> tables;
    tables.emplace("shape_params", new std::ofstream("/home/pullen/analysis/B02DKstar/"
                "ANA_resources/Tables/Data_fit/fit_result_shape_params.tex"));
    tables.emplace("obs", new std::ofstream("/home/pullen/analysis/B02DKstar/"
            "ANA_resources/Tables/Data_fit/fit_results_obs.tex"));
    for (auto table : tables) {
        *table.second << "\\begin{table}[h]" << std::endl;
        *table.second << "  \\centering" << std::endl;
        *table.second << "  \\begin{tabular}{lc}" << std::endl;
        *table.second << "      \\toprule" << std::endl;
        *table.second << "      Parameter & Value \\\\" << std::endl;
        *table.second << "      \\midrule" << std::endl;
    }

    // Map of Latex math style parameter names
    std::map<TString, TString> names;
    names.emplace("pdf_params_4body_signal_sigma_L", "\\sigma_L^{\\mathrm{4body}}");
    names.emplace("pdf_params_A_KK_Bs", "\\mathcal{A}_{KK}^s");
    names.emplace("pdf_params_A_KK_low", "\\mathcal{A}_{KK}^{\\mathrm{low}}");
    names.emplace("pdf_params_A_KK_signal_blind", "\\mathcal{A}_{KK}");
    names.emplace("pdf_params_A_Kpi_low", "\\mathcal{A}_{K\\pi}^{\\mathrm{low}}");
    names.emplace("pdf_params_A_Kpi_signal", "\\mathcal{A}_{K\\pi}");
    names.emplace("pdf_params_A_Kpipipi_low", "\\mathcal{A}_{K\\pi\\pi\\pi}^{\\mathrm{low}}");
    names.emplace("pdf_params_A_Kpipipi_signal", "\\mathcal{A}_{K\\pi\\pi\\pi}");
    names.emplace("pdf_params_A_piK_Bs", "\\mathcal{A}_{\\pi K}^s");
    names.emplace("pdf_params_A_piKpipi_Bs", "\\mathcal{A}_{\\pi K\\pi\\pi}^s");
    names.emplace("pdf_params_A_pipi_Bs", "\\mathcal{A}_{\\pi\\pi}^s");
    names.emplace("pdf_params_A_pipi_low", "\\mathcal{A}_{\\pi\\pi}^{\\mathrm{low}}");
    names.emplace("pdf_params_A_pipi_signal_blind", "\\mathcal{A}_{\\pi\\pi}");
    names.emplace("pdf_params_A_pipipipi_Bs", "\\mathcal{A}_{\\pi\\pi\\pi\\pi}^s");
    names.emplace("pdf_params_A_pipipipi_low", "\\mathcal{A}_{\\pi\\pi\\pi\\pi}" "^\\mathrm{low}");
    names.emplace("pdf_params_A_pipipipi_signal_blind", "\\mathcal{A}_{\\pi\\pi\\pi\\pi}");
    names.emplace("pdf_params_BF_R_Bs_low_vs_Bs", "R_\\mathrm{BF}^{B^0_s\\ \\mathrm{low}}");
    names.emplace("pdf_params_BF_R_low_vs_signal", "R_\\mathrm{BF}^{\\mathrm{low}}");
    names.emplace("pdf_params_Bs_low_frac_010", "\\beta_{010}");
    names.emplace("pdf_params_Bs_sigma_L", "\\sigma_L^s");
    names.emplace("pdf_params_R_KK_vs_Kpi_low", "\\mathcal{R}_{KK}^\\mathrm{low}");
    names.emplace("pdf_params_R_KK_vs_Kpi_signal_blind", "\\mathcal{R}_{KK}");
    names.emplace("pdf_params_R_KK_vs_piK_Bs", "\\mathcal{R}_{KK}^s");
    names.emplace("pdf_params_R_minus_DKpipi_piK", "\\mathcal{R}_{DK\\pi\\pi}^-");
    names.emplace("pdf_params_R_minus_low_piK", "\\mathcal{R}_\\mathrm{low}^-");
    names.emplace("pdf_params_R_minus_low_piKpipi", "\\mathcal{R}_\\mathrm{low}^{-\\ " "\\mathrm{4body}}");
    names.emplace("pdf_params_R_minus_signal_blind_piK", "\\mathcal{R}_d^-");
    names.emplace("pdf_params_R_minus_signal_blind_piKpipi", "\\mathcal{R}_d^{-\\ " "\\mathrm{4body}}");
    names.emplace("pdf_params_R_pipi_vs_Kpi_low", "\\mathcal{R}_{\\pi\\pi}^\\mathrm{low}");
    names.emplace("pdf_params_R_pipi_vs_Kpi_signal_blind", "\\mathcal{R}_{\\pi\\pi}");
    names.emplace("pdf_params_R_pipi_vs_piK_Bs", "\\mathcal{R}_{\\pi\\pi}^s");
    names.emplace("pdf_params_R_pipipipi_vs_Kpipipi_low", "\\mathcal{R}_{\\pi\\pi\\pi\\pi}" "^\\mathrm{low}");
    names.emplace("pdf_params_R_pipipipi_vs_Kpipipi_signal_blind", "\\mathcal{R}_" "{\\pi\\pi\\pi\\pi}");
    names.emplace("pdf_params_R_pipipipi_vs_piKpipi_Bs", "\\mathcal{R}_{\\pi\\pi\\pi\\pi}^s");
    names.emplace("pdf_params_R_plus_DKpipi_piK", "\\mathcal{R}_{DK\\pi\\pi}^+");
    names.emplace("pdf_params_R_plus_low_piK", "\\mathcal{R}_\\mathrm{low}^+");
    names.emplace("pdf_params_R_plus_low_piKpipi", "\\mathcal{R}_\\mathrm{low}^{+\\ " "\\mathrm{4body}}");
    names.emplace("pdf_params_R_plus_signal_blind_piK", "\\mathcal{R}_d^+");
    names.emplace("pdf_params_R_plus_signal_blind_piKpipi", "\\mathcal{R}_d^{+\\ " "\\mathrm{4body}}");
    names.emplace("pdf_params_four_vs_two_body_ratio", "R_{\\mathrm{4body}}");
    names.emplace("pdf_params_low_frac_010_GLW_minus", "\\alpha_{010\\ \\mathrm{GLW}}^-");
    names.emplace("pdf_params_low_frac_010_GLW_plus", "\\alpha_{010\\ \\mathrm{GLW}}^+");
    names.emplace("pdf_params_low_frac_010_Kpi", "\\alpha_{010,\\ K\\pi}");
    names.emplace("pdf_params_low_frac_010_Kpipipi", "\\alpha_{010,\\ K\\pi\\pi\\pi}");
    names.emplace("pdf_params_low_frac_010_piK_minus", "\\alpha_{010,\\ \\pi K}^-");
    names.emplace("pdf_params_low_frac_010_piK_plus", "\\alpha_{010,\\ \\pi K}^+");
    names.emplace("pdf_params_low_frac_010_piKpipi_minus", "\\alpha_{010,\\ " "\\pi K\\pi\\pi}^-");
    names.emplace("pdf_params_low_frac_010_piKpipi_plus", "\\alpha_{010,\\ \\pi K\\pi\\pi}^+");
    names.emplace("pdf_params_low_frac_010_pipipipi_minus", "\\alpha_{010,\\ " "\\pi\\pi\\pi\\pi}^-");
    names.emplace("pdf_params_low_frac_010_pipipipi_plus", "\\alpha_{010,\\ " "\\pi\\pi\\pi\\pi}^+");
    names.emplace("pdf_params_n_Bs_piK", "n_{\\pi K}^{B_s}");
    names.emplace("pdf_params_n_Bs_piKpipi", "n_{\\pi K\\pi\\pi}^{B_s}");
    names.emplace("pdf_params_n_expo_KK", "n_{KK}^\\mathrm{comb}");
    names.emplace("pdf_params_n_expo_Kpi", "n_{K\\pi}^\\mathrm{comb}");
    names.emplace("pdf_params_n_expo_Kpipipi", "n_{K\\pi\\pi\\pi}^\\mathrm{comb}");
    names.emplace("pdf_params_n_expo_piK", "n_{\\pi K}^\\mathrm{comb}");
    names.emplace("pdf_params_n_expo_piKpipi", "n_{\\pi K\\pi\\pi}^\\mathrm{comb}");
    names.emplace("pdf_params_n_expo_pipi", "n_{\\pi\\pi}^\\mathrm{comb}");
    names.emplace("pdf_params_n_expo_pipipipi", "n_{\\pi\\pi\\pi\\pi}^\\mathrm{comb}");
    names.emplace("pdf_params_n_rho_Kpi", "n_{K\\pi}^{\\rho^0}");
    names.emplace("pdf_params_n_rho_Kpipipi", "n_{K\\pi\\pi\\pi}^{\\rho^0}");
    names.emplace("pdf_params_n_signal_Kpi", "n_{K\\pi}");
    names.emplace("pdf_params_n_signal_Kpipipi", "n_{K\\pi\\pi\\pi}");
    names.emplace("pdf_params_shift", "\\delta");
    names.emplace("pdf_params_signal_sigma_L", "\\sigma_L^\\mathrm{2body}");
    names.emplace("pdf_params_slope_KK", "c_{KK}");
    names.emplace("pdf_params_slope_Kpi", "c_{K\\pi}");
    names.emplace("pdf_params_slope_Kpipipi", "c_{K\\pi\\pi\\pi}");
    names.emplace("pdf_params_slope_pipi", "c_{\\pi\\pi}");
    names.emplace("pdf_params_slope_pipipipi", "c_{\\pi\\pi\\pi\\pi}");

    // Fill table
    RooArgList pars = r->floatParsFinal();
    TIterator * it = pars.createIterator();
    RooRealVar * var;
    std::ofstream * table;
    while ((var = (RooRealVar*)it->Next())) {
        table = tables["shape_params"];
        std::string var_string = std::string(var->GetName());
        if (var_string.find("pdf_params_A_") == 0) table = tables["obs"];
        if (var_string.find("pdf_params_R_") == 0) table = tables["obs"];
        std::pair<int, int> precision = getPrecision(var->getVal(), var->getError());
        *table << "      ";
        *table << "$" << names[var->GetName()] << "$ & ";
        if (var_string.find("blind") == std::string::npos) {
            if (var->getError() > 1) {
                *table << std::fixed << std::setprecision(0);
                *table << var->getVal() << " $\\pm$ " << var->getError() << " \\\\";
            } else {
                if (precision.first < 0) {
                    *table << "0.0";
                } else {
                    *table << std::setprecision(precision.first);
                    *table << var->getVal();
                }
                *table << " $\\pm$ "; 
                *table << std::setprecision(precision.second);
                *table << var->getError() << " \\\\" << std::endl;
            }
        } else {
            *table << "BLIND \\\\" << std::endl;
        }
    }

    // Finish table
    for (auto table : tables) {
        *table.second << "      \\bottomrule" << std::endl;
        *table.second << "  \\end{tabular}" << std::endl;
    }

    // Caption and label for shapes table
    *tables["shape_params"] << "  \\caption{Results for floating shape parameters "
        "from a fit to selected candidates. The meanings of the parameters are "
        "given in Table~\\ref{tab:floating_pars_description}.}." << std::endl;
    *tables["shape_params"] << "\\label{tab:floating_pars_shapes}" << std::endl;

    // Caption and label for observables table
    *tables["obs"] << "  \\caption{Results for ratios and asymmetries  "
        "from a fit to selected candidates. The meanings of the parameters are "
        "given in Table~\\ref{tab:floating_pars_description}.}." << std::endl;
    *tables["obs"] << "\\label{tab:floating_pars_obs}" << std::endl;

    for (auto table : tables) {
        *table.second << "\\end{table}" << std::endl;
        table.second->close();
    }
    result_file->Close();
}
