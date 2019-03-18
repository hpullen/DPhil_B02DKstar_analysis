#include "../../names.h"

// Get correlation matrix from RooFitResult
void statistical_correlation() {

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

    // Open output file and start table
    std::ofstream file("../../../ANA_resources/Tables/Correlation/stat.tex");
    std::string tab_format = "l|";
    for (std::string par : obs) {
        tab_format += "r";
    }
    file << "\\begin{table}" << std::endl;
    file << "\\centering" << std::endl;
    file << "\\scriptsize" << std::endl;
    file << "\\caption{Statistical correlation matrix for the measured observables.}"
        << std::endl;
    file << "\\begin{tabular}{" << tab_format << "}" << std::endl; 
    for (std::string par : obs) {
        file << "& " << paper_names[par];
    }
    file << " \\\\" << std::endl;
    file << "\\midrule" << std::endl;

    // Loop through and print
    for (std::string par1 : obs) {
        file << "\n" << paper_names[par1];

        for (std::string par2 : obs) {
            double corr = r->correlation(("pdf_params_" + par1).c_str(),
                    ("pdf_params_" + par2).c_str());
            file << "& " << std::setprecision(2) << std::fixed << corr;
        }
        file << " \\\\" << std::endl;
    }

    // End table and close
    file << "\\end{tabular}" << std::endl;
    file << "\\end{table}" << std::endl;
    file << "\\label{tab:stat_correlation}" << std::endl;
    file.close();
    tfile->Close();
}
