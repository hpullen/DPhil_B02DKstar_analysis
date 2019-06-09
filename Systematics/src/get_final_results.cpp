#include <iostream>
#include <fstream>

#include "TFile.h"
#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"

#include "ParameterReader.hpp"

// ==========================================
// Get final results and print them to a file
// ==========================================
int main(int argc, char * argv[]) {

    // Combined runs?
    bool combine_runs = false;
    bool split_obs = false;
    if (argc > 1) {
        if (std::string(argv[1]) == "--combineRuns") {
            combine_runs = true;
            std::cout << "Combined runs result" << std::endl;
        } else if (std::string(argv[1]) == "--splitObs") {
            split_obs = true;
            std::cout << "Split observables result" << std::endl;
        }
    }

    // List of parameters
    std::vector<std::string> pars = {};
    if (!split_obs) {
        pars.push_back("A_signal_Kpi");
        pars.push_back("A_signal_Kpipipi");
        pars.push_back("A_Bs_piK");
        pars.push_back("A_Bs_piKpipi");
        pars.push_back("R_signal_KK");
        pars.push_back("A_signal_KK");
        pars.push_back("R_Bs_KK_run1");
        pars.push_back("R_Bs_KK_run2");
        pars.push_back("A_Bs_KK");
        pars.push_back("R_signal_pipi");
        pars.push_back("A_signal_pipi");
        pars.push_back("R_Bs_pipi_run1");
        pars.push_back("R_Bs_pipi_run2");
        pars.push_back("A_Bs_pipi");
        pars.push_back("R_signal_piK_minus");
        pars.push_back("R_signal_piK_plus");
        pars.push_back("R_signal_piKpipi_minus");
        pars.push_back("R_signal_piKpipi_plus");
        if (!combine_runs) {
            pars.push_back("R_signal_pipipipi");
            pars.push_back("A_signal_pipipipi");
            pars.push_back("A_Bs_pipipipi");
            pars.push_back("R_Bs_pipipipi_run2");
        }
        pars.push_back("R_ADS_piK");
        pars.push_back("A_ADS_piK");
        pars.push_back("R_ADS_piKpipi");
        pars.push_back("A_ADS_piKpipi");
    } else {
        pars.push_back("A_signal_Kpi_run1");
        pars.push_back("A_signal_Kpi_run2");
        pars.push_back("A_signal_Kpipipi_run1");
        pars.push_back("A_signal_Kpipipi_run2");
        pars.push_back("A_Bs_piK_run1");
        pars.push_back("A_Bs_piK_run2");
        pars.push_back("A_Bs_piKpipi_run1");
        pars.push_back("A_Bs_piKpipi_run2");
        pars.push_back("R_signal_KK_run1");
        pars.push_back("A_signal_KK_run1");
        pars.push_back("R_Bs_KK_run1");
        pars.push_back("A_Bs_KK_run1");
        pars.push_back("R_signal_pipi_run1");
        pars.push_back("A_signal_pipi_run1");
        pars.push_back("R_Bs_pipi_run1");
        pars.push_back("A_Bs_pipi_run1");
        pars.push_back("R_signal_pipi_run2");
        pars.push_back("A_signal_pipi_run2");
        pars.push_back("R_Bs_pipi_run2");
        pars.push_back("A_Bs_pipi_run2");
        pars.push_back("R_signal_KK_run2");
        pars.push_back("A_signal_KK_run2");
        pars.push_back("R_Bs_KK_run2");
        pars.push_back("A_Bs_KK_run2");
        pars.push_back("R_signal_pipipipi_run2");
        pars.push_back("A_signal_pipipipi_run2");
        pars.push_back("R_Bs_pipipipi_run2");
        pars.push_back("A_Bs_pipipipi_run2");
        pars.push_back("R_signal_piK_run1_minus");
        pars.push_back("R_signal_piK_run1_plus");
        pars.push_back("R_signal_piKpipi_run1_minus");
        pars.push_back("R_signal_piKpipi_run1_plus");
        pars.push_back("R_signal_piK_run2_minus");
        pars.push_back("R_signal_piK_run2_plus");
        pars.push_back("R_signal_piKpipi_run2_minus");
        pars.push_back("R_signal_piKpipi_run2_plus");
    }

    // Store R_Bs for averaging
    std::map<std::string, double> R_Bs_vals;
    std::map<std::string, double> R_Bs_stat;
    std::map<std::string, double> R_Bs_sys;
    std::map<std::string, double> R_Bs_biases;

    // List of systematics sources
    std::vector<std::string> sources = {
        "branching_ratios",
        "selection_efficiency",
        "PID",
        "production_asymmetry",
        "detection_asymmetry",
        // "fs_fd",
        "rho_PID",
        "signal_shape_pars",
        "background_shape_pars",
        "Bs_low_shape_pars",
        "gamma_pi_inputs",
        "DKpipi_inputs",
        "charmless"
        // "bias_correction"
    };

    // Open output file
    std::string out_file = "final_result.param";
    if (combine_runs) out_file = "final_result_combinedRuns.param";
    if (split_obs) out_file = "final_result_splitObs.param";
    std::ofstream file(out_file);

    // Open separate ADS file
    std::ofstream ADS_file("ADS_results.param");

    // Open fit result file
    std::string in_file = "../Fit_data/Results/twoAndFourBody_data_split";
    if (combine_runs) in_file += "_combinedRuns";
    if (split_obs) in_file += "_splitObs";
    in_file += ".root";
    TFile * res_file = TFile::Open(in_file.c_str(), "READ");
    RooFitResult * r = (RooFitResult*)res_file->Get("fit_result");
    RooWorkspace * wspace = (RooWorkspace*)res_file->Get("wspace");

    // Read in systematics for each source
    ParameterReader * pr = new ParameterReader("..");
    std::string dir = combine_runs ? "/CombinedRuns/" : "";
    if (split_obs) dir = "/SplitObs/";
    for (auto source : sources) {
        pr->ReadParameters("Systematics/Results/" + dir + source + ".param", source);
    }

    // Read in pull biases
    std::string extra = combine_runs ? "_combinedRuns" : "";
    if (split_obs) extra = "_splitObs";
    pr->ReadParameters("Biases/biases" + extra + ".param", "bias");

    // File to output bias correction systematics and final systematic
    std::ofstream bias_file("Results/" + dir + "bias_correction.param");
    std::ofstream bias_file_all("Results/all/bias_correction" + extra + ".param");
    std::ofstream sum_file("Results/" + dir + "sum.param");

    // Loop through parameters
    for (auto par : pars) {

        // Get properties
        bool blind = false;
        bool is_ADS = false;
        bool is_R_Bs = false;
        std::string par_short = par;
        if (par.find("_blind") != std::string::npos) {
            blind = true;
            par_short = par.substr(0, par.find("_blind"));
        }
        if (par.find("R_Bs") == 0) {
            is_R_Bs = true;
        } else if (par.find("R_ADS") == 0 || par.find("A_ADS") == 0) {
            is_ADS = true;
        }

        // Extract parameter from file
        double val;
        double stat;
        if (!is_R_Bs && !is_ADS) {
            RooRealVar * var = (RooRealVar*)r->floatParsFinal().find(("pdf_params_" 
                        + par).c_str());
            val = var->getVal();
            stat = var->getError();
        } else {
            RooFormulaVar * var = (RooFormulaVar*)wspace->arg((
                        "pdf_params_" + par).c_str());
            val = var->getVal();
            stat = var->getPropagatedError(*r);
        }

        // Sum total systematics
        double sq_sum_sys = 0;
        for (auto source : sources) {
            if (pr->Contains(par, source)) {
                std::cout << "Adding to " << par << " from source " << source
                    << ": " << pr->GetValue(par, source) << std::endl;
                sq_sum_sys += pow(pr->GetValue(par, source), 2); 
            }
        }
        double sys = sqrt(sq_sum_sys);

        // Apply bias correction if needed
        if (pr->Contains(par_short, "bias")) {
            double bias = pr->GetValue(par_short, "bias");
            double bias_err = pr->GetError(par_short, "bias");
            if (fabs(bias/bias_err) >= 3.0) {
                std::cout << "Applying correction to " << par_short << std::endl;
                std::cout << "Previous value: " << val << " +/- " << stat << 
                    " +/- " << sys << std::endl;
                std::cout << "Bias: " << bias << " +/- " << bias_err << std::endl;
                val = val + bias * stat;
                double bias_sys_err = bias_err * stat;
                bias_file << par << " " << bias_sys_err << std::endl;
                sys = sqrt(sys*sys + bias_sys_err*bias_sys_err);
                std::cout << "New value: " << val << " +/- " << stat << 
                    " +/- " << sys << std::endl;
                if (is_R_Bs) {
                    R_Bs_biases.emplace(par, bias_sys_err);
                }
            } else {
                if (is_R_Bs) {
                    R_Bs_biases.emplace(par, 0);
                }
            }
        }

        // Print total summed systematic to file
        sum_file << par << " " << sys << std::endl;

        // Print result to file
        if ((!split_obs && !is_ADS && !is_R_Bs) || (split_obs && !is_ADS)) {
            if (blind) {
                file << par << " ?? " << stat << " " << sys << std::endl;
            } else {
                file << par << " " << val << " " << stat << " " << sys << std::endl;
            }
        } else if (is_ADS) {
            ADS_file << par << " " << val << " " << stat << " " << sys << std::endl;
        } else {
            R_Bs_vals.emplace(par, val);
            R_Bs_stat.emplace(par, stat);
            R_Bs_sys.emplace(par, sys);
        }
    }

    // Average R_Bs if not splitting observables
    if (!split_obs) {

        // Average for KK and pipi
        double frac_1 = 1530.0/(1530 + 2200);
        double frac_2 = 2200.0/(1530 + 2200);
        for (std::string mode : {"KK", "pipi"}) {
            double val_1 = R_Bs_vals["R_Bs_" + mode + "_run1"];
            double val_2 = R_Bs_vals["R_Bs_" + mode + "_run2"];
            double val = frac_1 * val_1 + frac_2 * val_2;
            double stat_1 = R_Bs_stat["R_Bs_" + mode + "_run1"];
            double stat_2 = R_Bs_stat["R_Bs_" + mode + "_run2"];
            double stat = sqrt(pow(stat_1 * frac_1, 2) + pow(stat_2 * frac_2, 2));
            double sys_1 = R_Bs_sys["R_Bs_" + mode + "_run1"];
            double sys_2 = R_Bs_sys["R_Bs_" + mode + "_run2"];
            double sys = sqrt(pow(sys_1 * frac_1, 2) + pow(sys_2 * frac_2, 2));
            double bias_1 = R_Bs_biases["R_Bs_" + mode + "_run1"];
            double bias_2 = R_Bs_biases["R_Bs_" + mode + "_run2"];
            double bias = sqrt(pow(bias_1 * frac_1, 2) + pow(bias_2 * frac_2, 2));
            file << "R_Bs_" + mode << " " << val << " " << stat << " " << sys 
                << std::endl;
            sum_file << "R_Bs_" + mode << " " << sys << std::endl;
            if (bias > 0.000000000001) {
                bias_file << "R_Bs_" + mode << " " << bias << std::endl;
            }
        }

        // Print pipipipi value
        file << "R_Bs_pipipipi " << R_Bs_vals["R_Bs_pipipipi_run2"]
            << " " << R_Bs_stat["R_Bs_pipipipi_run2"] << " "
            << R_Bs_sys["R_Bs_pipipipi_run2"] << std::endl;
            sum_file << "R_Bs_pipipipi" << " " << R_Bs_sys["R_Bs_pipipipi_run2"] << std::endl;
    }
}
