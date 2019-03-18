#include "../../names.h"

// Get correlation matrix from RooFitResult
void systematic_correlation() {

    // Open file and fit result
    TFile * tfile = TFile::Open("~/analysis/B02DKstar/Fit_data/"
            "Results/twoAndFourBody_data_split.root");
    RooFitResult * r = (RooFitResult*)tfile->Get("fit_result");

    // List of observables
    std::vector<std::string> obs = {
        "A_signal_KK",
        "A_signal_pipi",
        "R_signal_KK", 
        "R_signal_pipi",
        "A_signal_pipipipi",
        "R_signal_pipipipi",
        "R_signal_piK_plus",
        "R_signal_piK_minus",
        "R_signal_piKpipi_plus",
        "R_signal_piKpipi_minus",
        "A_signal_Kpi",
        "A_signal_Kpipipi"
    };
    
    // Sources of systematic uncertainty
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
        "DKpipi_inputs"
    };

    // Open output file and start table
    std::ofstream file("../../../ANA_resources/Tables/Correlation/sys.tex");
    std::string tab_format = "l|";
    for (std::string par : obs) {
        tab_format += "r";
    }
    file << "\\begin{table}" << std::endl;
    file << "\\centering" << std::endl;
    file << "\\scriptsize" << std::endl;
    file << "\\caption{Systematic correlation matrix for the measured observables.}"
        << std::endl;
    file << "\\begin{tabular}{" << tab_format << "}" << std::endl; 
    for (std::string par : obs) {
        file << "& " << paper_names[par];
    }
    file << " \\\\" << std::endl;
    file << "\\midrule" << std::endl;

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

    // Loop through and print
    for (std::string par1 : obs) {
        file << "\n" << paper_names[par1];
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
                            ":sys_signal_final_value_" + par2 + ">>+" + 
                            name).c_str());
                TH2F * hist = (TH2F*)gDirectory->Get(name.c_str());
                corr = hist->GetCorrelationFactor();
                delete hist;
            }

            // Print to file
            file << "& " << std::setprecision(2) << std::fixed << corr;
        }
        file << " \\\\" << std::endl;
    }

    // End table and close
    file << "\\end{tabular}" << std::endl;
    file << "\\end{table}" << std::endl;
    file << "\\label{tab:sys_correlation}" << std::endl;
    file.close();
    tfile->Close();
}
