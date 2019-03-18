// Get systematic correlations
void sys_correlation(int bod = 2) {

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
    std::ofstream file("correlations/sys_correlation" + extra + ".txt");
    std::ofstream cmd_file("correlations/sys_correlation" + extra + ".C");

    // Sources
    std::vector<std::string> sources = {
        "branching_ratios",
        "selection_efficiency",
        "PID",
        "production_asymmetry",
        "detection_asymmetry",
        "detection_asymmetry",
        "signal_shape_pars",
        "Bs_low_shape_pars",
        "background_shape_pars",
        "gamma_pi_inputs",
        "DKpipi_inputs"};

    // Open tree
    std::string dir = "/data/lhcb/users/pullen/B02DKstar/systematics/";
    TChain * tree = new TChain("sys_tree");
    for (auto source : sources) {
        tree->Add((dir + source + "/*.root").c_str());
    }

    // Open charmless tree
    TChain * charmless_tree = new TChain("toy_tree");
    charmless_tree->Add((dir + "charmless/*.root").c_str());

    // Map for results
    std::map<std::string, std::map<std::string, double>> results;

    // Title line
    file << "OBSERVABLE";
    for (std::string par : obs) {
        file << " " << par;
    }
    cmd_file << "const double systCorrelations[49] = {";

    // Loop through and print
    TCanvas * canv = new TCanvas("canvas", "", 600, 600);
    for (std::string par1 : obs) {
        file << "\n" << par1; 
        cmd_file << "\n";
        for (std::string par2 : obs) {

            // See if result already exists
            double corr;
            bool found = false;
            if (results.find(par2) != results.end()) {
                if (results[par2].find(par1) != results[par2].end()) {
                    corr = results[par2][par1];
                    found = true;
                }
            }

            // Get correlation if not already calculated
            if (!found) {
                std::string name = (par1 + "_" + par2.c_str());
                tree->Draw((par1 + ":" + par2 
                            + ">>" + name).c_str());
                charmless_tree->Draw(("sys_signal_final_value_" + par1 + 
                            ":sys_signal_final_value_" + par2 + ">>+" + name).c_str());
                TH2F * hist = (TH2F*)gDirectory->Get(name.c_str());
                corr = hist->GetCorrelationFactor();

                // Save plot
                hist->Draw();
                canv->SaveAs(("correlations/Plots/" + name + ".pdf").c_str());
                delete hist;
                canv->Clear();

                // Add to map
                results[par1][par2] = corr;
            }

            // Print to file
            file << " " << corr;
            cmd_file << corr << ", ";

        }
        cmd_file << std::endl;
    }
    cmd_file << "};" << std::endl;
    file << std::endl;
    cmd_file.close();
    file.close();
}
