#include "../ANA_scripts/names.h"

// Make correlation matrix with both statistical and systematic correlations
void make_matrix(bool split_obs = false) {

    // Extra string and directory
    std::string extra = split_obs ? "_splitObs" : "";
    std::string extra_dir = split_obs ? "/SplitObs/" : "";

    // Open file and fit result
    TFile * tfile = TFile::Open(("../Fit_data/Results/"
            "twoAndFourBody_data_split" + extra + ".root").c_str());
    RooFitResult * r = (RooFitResult*)tfile->Get("fit_result");

    // Read in systematic uncertainties
    std::ifstream sys_err_file("../Systematics/Results/"
            + extra_dir + "sum.param");
    std::string word;
    std::string name;
    std::string err;
    std::map<std::string, double> sys_errs;
    while (sys_err_file >> word) {
        name = word;
        sys_err_file >> word;
        err = word;
        sys_errs[name] = std::stod(err);
    }
    sys_err_file.close();

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

    // Open trees
    std::string dir = "/data/lhcb/users/pullen/B02DKstar/systematics/" + extra_dir;
    TChain * tree = new TChain("sys_tree");
    for (auto source : sources) {
        tree->Add((dir + source + "/*.root").c_str());
    }
    TChain * charmless_tree = new TChain("toy_tree");
    charmless_tree->Add((dir + "charmless/*.root").c_str());

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
        "R_signal_piKpipi_minus"
    };
    
    // Add run indicators if splitting
    if (split_obs) {
        auto temp_obs = obs;
        obs = {};
        for (std::string run : {"_run1", "_run2"}) {
            for (auto par : temp_obs) {
                std::string new_par = par + run;
                if (par.find("pipipipi") != std::string::npos
                        && run == "_run1") continue;
                if (par.find("R_signal_piK") != std::string::npos) {
                    for (std::string sign : {"_plus", "_minus"}) {
                        if (par.find(sign) != std::string::npos) {
                            new_par = par.substr(0, par.find(sign)) 
                                + run + sign;
                        }
                    }
                }
                obs.push_back(new_par);
            }
        }
    }

    // Open output files
    std::map<std::string, std::ofstream*> files;
    files["both"] = new std::ofstream("Results/correlation" + extra + ".txt");
    files["stat"] = new std::ofstream("Results/correlation_stat" + extra + ".txt");
    files["sys"] = new std::ofstream("Results/correlation_sys" + extra + ".txt");
    files["cov"] = new std::ofstream("Results/covariance" + extra + ".txt");
    std::ofstream tex_file("Results/correlation" + extra + ".tex");

    // Title line
    for (std::string par : obs) {
        for (auto file : files) {
            *file.second << " " << par;
        }
    }

    // Begin tex file
    std::string tab_format = "l|";
    for (std::string par : obs) {
        tab_format += "r";
    }
    std::string tab_type = split_obs ? "sidewaystable" : "table";
    tex_file << "\\begin{" << tab_type << "}" << std::endl;
    tex_file << "\\centering" << std::endl;
    std::string extra_cap = split_obs ? ", split by LHC running period" : "";
    tex_file << "\\caption{Correlation matrix for the principal observables"
        << extra_cap << ".}" << std::endl;
    tex_file << "\\begin{tabular}{" << tab_format << "}" << std::endl; 
    for (std::string par : obs) {
        tex_file << "& " << paper_names[par];
    }
    tex_file << " \\\\" << std::endl;
    tex_file << "\\midrule" << std::endl;

    // Maps for results
    std::map<std::string, std::map<std::string, 
        std::map<std::string, double>>> results;

    // Get each correlation
    for (std::string par1 : obs) {

        // Name of parameter
        for (auto file : files) {
            *file.second << "\n" << par1;
        }
        tex_file << "\n" << paper_names[par1];

        // Loop through all parameters
        for (std::string par2 : obs) {

            // Results
            double corr_stat;
            double corr_sys;
            double cov;
            double corr;

            // See if result already exists
            bool found = false;
            if (results["both"].find(par2) != results["both"].end()) {
                if (results["both"][par2].find(par1) != 
                        results["both"][par2].end()) {
                    corr_stat = results["stat"][par2][par1];
                    corr_sys = results["sys"][par2][par1];
                    cov = results["cov"][par2][par1];
                    corr = results["both"][par2][par1];
                    found = true;
                }
            }

            // Calculate results if needed
            if (!found) {

                // Statistical correlation and covariance
                RooRealVar * var1 = (RooRealVar*)r->floatParsFinal().find((
                            "pdf_params_" + par1).c_str());
                RooRealVar * var2 = (RooRealVar*)r->floatParsFinal().find((
                            "pdf_params_" + par2).c_str());
                corr_stat = r->correlation(("pdf_params_" + par1).c_str(),
                        ("pdf_params_" + par2).c_str());
                double stat_err1 = var1->getError();
                double stat_err2 = var2->getError();
                double cov_stat = corr_stat * stat_err1 * stat_err2;

                // Systematic correlation and covariance
                std::string name = (par1 + "_" + par2.c_str());
                tree->Draw((par1 + ":" + par2 
                            + ">>" + name).c_str());
                charmless_tree->Draw(("sys_signal_final_value_" + par1 + 
                            ":sys_signal_final_value_" + par2 + ">>+" + 
                            name).c_str());
                TH2F * hist = (TH2F*)gDirectory->Get(name.c_str());
                corr_sys = hist->GetCorrelationFactor();
                delete hist;
                double sys_err1 = sys_errs[par1];
                double sys_err2 = sys_errs[par2];
                double cov_sys = corr_sys * sys_err1 * sys_err2;

                // Total covariance and correlation
                cov = cov_stat + cov_sys;
                double err_tot1 = sqrt(stat_err1 * stat_err1 + sys_err1 * sys_err1);
                double err_tot2 = sqrt(stat_err2 * stat_err2 + sys_err2 * sys_err2);
                corr = cov / (err_tot1 * err_tot2);

                // Save in map
                results["stat"][par1][par2] = corr_stat;
                results["sys"][par1][par2] = corr_sys;
                results["cov"][par1][par2] = cov;
                results["both"][par1][par2] = corr;
            }

            // Print to files
            *files["stat"] << " " << corr_stat;
            *files["sys"] << " " << corr_sys;
            *files["cov"] << " " << cov;
            *files["both"] << " " << corr;
            tex_file << "& " << std::setprecision(2) << std::fixed << corr;

        }
        tex_file << " \\\\" << std::endl;
    }

    // Close files
    for (auto file : files) {
        file.second->close();
        tfile->Close();
    }
    tex_file << "\\end{tabular}" << std::endl;
    tex_file << "\\label{tab:correlation" << extra << "}" << std::endl;
    tex_file << "\\end{" << tab_type << "}" << std::endl;
    tex_file.close();

}
