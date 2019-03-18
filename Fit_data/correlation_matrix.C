// Get correlation matrix from RooFitResult
void correlation_matrix(int bod = 2) {

    // Open file and fit result
    TFile * tfile = TFile::Open("Results/twoAndFourBody_data_split.root");
    RooFitResult * r = (RooFitResult*)tfile->Get("fit_result");

    // List of observables
    std::vector<std::string> obs = {
        "A_signal_Kpi",
        "R_signal_piK_plus",
        "R_signal_piK_minus",
        "R_signal_KK",
        "R_signal_pipi",
        "A_signal_KK",
        "A_signal_pipi"
    };
    if (bod == 4) {
        obs = {
        "A_signal_Kpipipi",
        "R_signal_piKpipi_minus",
        "R_signal_piKpipi_plus",
        "R_signal_pipipipi",
        "A_signal_pipipipi"
        };
    }

    // Open output file
    std::string extra = (bod == 2) ? "" : "_4body";
    std::ofstream file("correlations/stat_correlation" + extra + ".txt");
    std::ofstream cmd_file("correlations/stat_correlation" + extra + ".C");

    // Title line
    file << "OBSERVABLE";
    for (std::string par : obs) {
        file << " " << par;
    }
    cmd_file << "const double statCorrelations[49] = {" << std::endl;

    // Loop through and print
    for (std::string par1 : obs) {
        file << "\n" << par1; 
        for (std::string par2 : obs) {
            double corr = r->correlation(("pdf_params_" + par1).c_str(),
                    ("pdf_params_" + par2).c_str());
            file << " " << corr;
            cmd_file << corr << ", ";
        }
        cmd_file << std::endl;
    }
    cmd_file << "};" << std::endl;
    file << std::endl;

    // Close
    file.close();
    cmd_file.close();
    tfile->Close();
}
