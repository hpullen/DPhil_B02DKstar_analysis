#include "../ANA_scripts/names.h"

// Make correlation matrix with both statistical and systematic correlations
void make_matrix(TString type = "") {

    // Extra string and directory
    std::string extra = "";
    std::string extra_out = "";
    if (type != "") extra += "_splitObs";
    if (type != "") extra_out += "_" + type;
    std::string extra_dir = "";
    if (type != "") extra_dir = "/SplitObs/"; 

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
        "R_signal_piKpipi_minus",
        "A_signal_Kpi",
        "A_signal_Kpipipi"
    };
    std::vector<std::string> obs2 = obs;
    
    // Add run indicators if splitting
    if (type != "") {
        auto temp_obs = obs;
        obs = {};
        obs2 = {};
        for (auto par : temp_obs) {

            // New parameter names
            std::string end1;
            std::string end2;
            if (type == "1v1") {
                end1 = "_run1";
                end2 = "_run1";
            } else if (type == "1v2") {
                end1 = "_run1";
                end2 = "_run2";
            } else {
                end1 = "_run2";
                end2 = "_run2";
            }
            std::string new_par1 = par + end1;
            std::string new_par2 = par + end2;

            // Deal with 4pi
            if (par.find("pipipipi") != std::string::npos) {
                if (type == "1v1") continue;
                if (type == "1v2") {
                    obs2.push_back(new_par2);
                    continue;
                }
            }
            
            // Deal with R_pm
            if (par.find("R_signal_piK") != std::string::npos) {
                for (std::string sign : {"_plus", "_minus"}) {
                    if (par.find(sign) != std::string::npos) {
                        new_par1 = par.substr(0, par.find(sign)) 
                            + end1 + sign;
                        new_par2 = par.substr(0, par.find(sign)) 
                            + end2 + sign;
                    }
                }
            }

            // Default setting
            obs.push_back(new_par1);
            obs2.push_back(new_par2);
        }
    }

    // Open output files
    std::map<std::string, std::ofstream*> files;
    files["both"] = new std::ofstream("Results/correlation" + extra_out + ".txt");
    files["stat"] = new std::ofstream("Results/correlation_stat" + extra_out + 
            ".txt");
    files["sys"] = new std::ofstream("Results/correlation_sys" + extra_out + ".txt");
    files["cov"] = new std::ofstream("Results/covariance" + extra_out + ".txt");
    std::ofstream tex_file("Results/correlation" + extra_out + ".tex");

    // Title line
    for (std::string par : obs2) {
        for (auto file : files) {
            *file.second << " " << par;
        }
    }

    // Begin tex file
    std::string tab_format = "l|";
    for (std::string par : obs2) {
        tab_format += "r";
    }
    std::string tab_type = "table";
    tex_file << "\\begin{" << tab_type << "}" << std::endl;
    tex_file << "\\centering" << std::endl;

    // Caption
    std::string extra_cap = "";
    if (type == "1v1") extra_cap = " in Run-1 data only";
    if (type == "2v2") extra_cap = " in Run-2 data only";
    if (type == "1v2") extra_cap = " between Run-1 and Run-2 data";
    tex_file << "\\caption{Combined statistical and systematic correlation matrix for the principal observables"
        << extra_cap << ".}" << std::endl;
    tex_file << "\\footnotesize" << std::endl;

    // Make tabular
    tex_file << "\\begin{tabular}{" << tab_format << "}" << std::endl; 
    for (std::string par : obs2) {
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
        for (std::string par2 : obs2) {

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
            tex_file << " & $" << std::setprecision(2) << std::fixed << corr
                << "$";

        }
        tex_file << " \\\\" << std::endl;
    }

    // Close files
    for (auto file : files) {
        file.second->close();
        tfile->Close();
    }
    tex_file << "\\end{tabular}" << std::endl;
    tex_file << "\\label{tab:correlation" << extra_out << "}" << std::endl;
    tex_file << "\\end{" << tab_type << "}" << std::endl;
    tex_file.close();

}
