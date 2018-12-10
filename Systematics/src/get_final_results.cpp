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

    // List of parameters
    std::vector<std::string> pars = {
        "R_signal_piK_minus_blind",
        "R_signal_piK_plus_blind",
        "R_signal_piKpipi_minus_blind",
        "R_signal_piKpipi_plus_blind",
        "A_signal_Kpi",
        "A_signal_Kpipipi",
        "A_Bs_piK",
        "A_Bs_piKpipi",
        "R_signal_KK_run1_blind",
        "A_signal_KK_run1_blind",
        "R_ds_KK_run1_blind",
        "A_Bs_KK_run1",
        "R_signal_pipi_run1_blind",
        "A_signal_pipi_run1_blind",
        "R_ds_pipi_run1_blind",
        "A_Bs_pipi_run1",
        "R_signal_pipi_run2_blind",
        "A_signal_pipi_run2_blind",
        "R_ds_pipi_run2_blind",
        "A_Bs_pipi_run2",
        "R_signal_KK_run2_blind",
        "A_signal_KK_run2_blind",
        "R_ds_KK_run2_blind",
        "A_Bs_KK_run2",
        "R_signal_pipipipi_run2_blind",
        "A_signal_pipipipi_run2_blind",
        "R_ds_pipipipi_run2_blind",
        "A_Bs_pipipipi_run2"
    };

    // List of systematics sources
    std::vector<std::string> sources = {
        "branching_ratios",
        "selection_efficiency",
        "PID",
        "production_asymmetry",
        "detection_asymmetry",
        "fs_fd",
        "signal_shape_pars",
        "background_shape_pars",
        "Bs_low_shape_pars",
        "four_vs_two",
        "gamma_pi_branching_ratios",
        "gamma_pi_selection",
        "DKpipi_inputs",
        "charmless",
        "bias_correction"
    };

    // Open output file
    std::ofstream file("final_result.param");

    // Open fit result file
    TFile * res_file = TFile::Open("../Fit_data/Results/"
            "twoAndFourBody_data_split.root", "READ");
    RooFitResult * r = (RooFitResult*)res_file->Get("fit_result");
    RooWorkspace * wspace = (RooWorkspace*)res_file->Get("wspace");

    // Read in systematics for each source
    ParameterReader * pr = new ParameterReader("..");
    for (auto source : sources) {
        pr->ReadParameters("Systematics/Results/" + source + ".param", source);
    }

    // Read in pull biases
    pr->ReadParameters("Biases/biases.param", "bias");

    // File to output bias correction systematics and final systematic
    std::ofstream bias_file("Results/bias_correction.param");
    std::ofstream bias_file_all("Results/all/bias_correction.param");
    std::ofstream sum_file("Results/sum.param");

    // Loop through parameters
    for (auto par : pars) {

        // Get properties
        bool blind = false;
        bool is_ds = false;
        std::string par_short = par;
        if (par.find("_blind") != std::string::npos) {
            blind = true;
            par_short = par.substr(0, par.find("_blind"));
        }
        if (par.find("R_ds") == 0) {
            is_ds = true;
        }

        // Extract parameter from file
        double val;
        double stat;
        if (!is_ds) {
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
            }
        }

        // Print total summed systematic to file
        sum_file << par << " " << sys << std::endl;

        // Print to file
        if (blind) {
            file << par << " ?? " << stat << " " << sys << std::endl;
        } else {
            file << par << " " << val << " " << stat << " " << sys << std::endl;
        }
    }
}
