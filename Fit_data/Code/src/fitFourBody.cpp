#include <iostream>
#include <fstream>
#include <vector>

#include "TApplication.h"
#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"

#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooCBShape.h"
#include "RooDataSet.h"
#include "RooExponential.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooUnblindUniform.h"
#include "RooHist.h"

#include "RooHORNSdini.h"
#include "RooHILLdini.h"
#include "RooLITTLEHORNSdini.h"

#include "ParameterReader.hpp"
#include "Plotter.hpp"

int main(int argc, char * argv[]) {

    // ===============
    // Read input args
    // ===============
    // Check for parameters
    if (argc != 4) {
        std::cout << "Usage: ./FitFourBody <2011:2012:2015:2016> <Sum: Y/N>"
            " <Binned: Y/N>" << std::endl;
        return -1;
    }

    // Year (colon separated string)
    std::string input_year = argv[1];

    // Sum over charge (Y/N)
    std::string sum = argv[2];

    // Binned vs. unbinned
    std::string binned = argv[3];

    // Vectors of years and D0 modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> modes = {"Kpipipi", "piKpipi"};

    // Include pipipipi if using Run 2
    if(input_year.find("2015") != std::string::npos || input_year.find("2016") != std::string::npos) {
        modes.push_back("pipipipi");
    }

    // ================
    // Set up variables
    // ================
    // Common parameters TO ADD: DOUBLE MIS ID VETO
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5000, 5800);
    RooRealVar KstarK_ID("KstarK_ID", "", -100000000, 100000000);
    RooRealVar BDT_Kpipipi("BDTG_Kpipipi_run2", "", 0.5, 1);
    RooRealVar BDT_pipipipi("BDTG_pipipipi_run2", "", 0.5, 1);

    // Make list of args to use
    std::map<std::string, RooArgList *> args;
    for (auto mode : modes) {
        args[mode] = new RooArgList();
        args[mode]->add(Bd_M);
        args[mode]->add(KstarK_ID);
        if (mode == "Kpipipi" || mode == "piKpipi") {
            args[mode]->add(BDT_Kpipipi);
        } else if (mode == "pipipipi") {
            args[mode]->add(BDT_pipipipi);
        }
    }

    // Set up bins
    int binWidth = 8;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // ============
    // Read in data
    // ============
    std::string path = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/";
    std::map<std::string, RooDataSet *> data_both;
    std::map<std::string, RooDataSet *> data_plus;
    std::map<std::string, RooDataSet *> data_minus;

    // Loop through modes
    for (auto mode : modes) {

        // Make TChain to hold data
        TChain * chain = new TChain("DecayTree");

        // Loop through years
        for (auto year : years) {

            // No 2011 or 2012 data for pipipipi
            if (mode == "pipipipi" && (year == "2011" || year == "2012")) continue;

            // Add data if year is in list
            if (input_year.find(year) != std::string::npos) {
                std::string filepath_down = path + year + "_down/" + mode + "_selected.root";
                std::string filepath_up = path + year + "_up/" + mode + "_selected.root";
                chain->Add(filepath_down.c_str());
                chain->Add(filepath_up.c_str());
            }
        }

        // Turn off irrelevant branches
        chain->SetBranchStatus("*", 0);
        chain->SetBranchStatus("Bd_ConsD_MD", 1);
        chain->SetBranchStatus("KstarK_ID", 1);     

        // Convert to RooDataSet
        std::cout << "Making RooDataSet for mode: " << mode << std::endl;
        data_both[mode] = new RooDataSet(("data_" + mode).c_str(), "", chain, *args[mode]);
        data_both[mode]->Print();

        // Get flavour separated modes if requested
        if (sum != "Y") {
            std::cout << "Splitting dataset by flavour." << std::endl;
            data_plus[mode] = (RooDataSet*)data_both[mode]->reduce("KstarK_ID > 0");
            data_minus[mode] = (RooDataSet*)data_both[mode]->reduce("KstarK_ID < 0");
            data_plus[mode]->Print();
            data_minus[mode]->Print();
        }
    }

    // =========================
    // Read in MC fit parameters
    // =========================
    ParameterReader pr;
    std::string MC_path = "/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/";
    pr.readParams("signal", MC_path + "signal_Kpipipi.param");
    pr.readParams("fourBody", MC_path + "signal_Kpi.param");
    pr.readParams("Bs", MC_path + "signal_Bs.param");
    pr.readParams("rho", MC_path + "rho_all_PIDcut.param");
    pr.readParams("low", MC_path + "lowMass.param");

    // =================
    // Make signal shape
    // =================
    // Shifts
    RooRealVar * delta_M = new RooRealVar("delta_M", "", 87.26, 80, 90);
    RooRealVar * shift = new RooRealVar("shift", "", 0, -10, 10);

    // Set up parameters
    RooRealVar * signal_mean_fixed = new RooRealVar("signal_mean_fixed", "", pr["signal_mean"]); 
    RooFormulaVar * signal_mean = new RooFormulaVar("signal_mean", "@0 + @1", RooArgList(*signal_mean_fixed, *shift));
    RooRealVar * signal_sigma_L = new RooRealVar("signal_sigma_L", "", pr["signal_sigma_L"]);
    RooRealVar * signal_sigma_ratio = new RooRealVar("signal_sigma_ratio", "", pr["signal_sigma_ratio"]);
    RooFormulaVar * signal_sigma_R = new RooFormulaVar("signal_sigma_R", "@0 * @1", RooArgList(*signal_sigma_L, *signal_sigma_ratio));
    RooRealVar * signal_alpha_L = new RooRealVar("signal_alpha_L", "", pr["signal_alpha_L"]);
    RooRealVar * signal_alpha_R = new RooRealVar("signal_alpha_R", "", pr["signal_alpha_R"]);
    RooRealVar * signal_n_L = new RooRealVar("signal_n_L", "", pr["signal_n_L"]);
    RooRealVar * signal_n_R = new RooRealVar("signal_n_R", "", pr["signal_n_R"]);
    RooRealVar * signal_frac = new RooRealVar("signal_frac", "", pr["signal_frac"]);

    // Make crystal ball PDFs
    RooCBShape * signal_CB_L = new RooCBShape("signal_CB_L", "", Bd_M, *signal_mean, 
            *signal_sigma_L, *signal_alpha_L, *signal_n_L);
    RooCBShape * signal_CB_R = new RooCBShape("signal_CB_R", "", Bd_M, *signal_mean, 
            *signal_sigma_R, *signal_alpha_R, *signal_n_R);
    RooAddPdf * signal_shape = new RooAddPdf("signal_shape", "", 
            RooArgList(*signal_CB_L, *signal_CB_R), RooArgList(*signal_frac));

    // =============
    // Make Bs shape
    // =============
    // Set up parameters
    RooFormulaVar * Bs_mean = new RooFormulaVar("Bs_mean", "@0 + @1", RooArgList(*signal_mean, *delta_M));
    RooRealVar * Bs_sigma_L = new RooRealVar("Bs_sigma_L", "", pr["Bs_sigma_L"]);
    RooRealVar * Bs_sigma_ratio = new RooRealVar("Bs_sigma_ratio", "", pr["Bs_sigma_ratio"]);
    RooFormulaVar * Bs_sigma_R = new RooFormulaVar("Bs_sigma_R", "@0 * @1", RooArgList(*Bs_sigma_L, *Bs_sigma_ratio));
    RooRealVar * Bs_alpha_L = new RooRealVar("Bs_alpha_L", "", pr["Bs_alpha_L"]);
    RooRealVar * Bs_alpha_R = new RooRealVar("Bs_alpha_R", "", pr["Bs_alpha_R"]);
    RooRealVar * Bs_n_L = new RooRealVar("Bs_n_L", "", pr["Bs_n_L"]);
    RooRealVar * Bs_n_R = new RooRealVar("Bs_n_R", "", pr["Bs_n_R"]);
    RooRealVar * Bs_frac = new RooRealVar("Bs_frac", "", pr["Bs_frac"]);

    // Make crystal ball PDFs
    RooCBShape * Bs_CB_L = new RooCBShape("Bs_CB_L", "", Bd_M, *Bs_mean, *Bs_sigma_L, *Bs_alpha_L, *Bs_n_L);
    RooCBShape * Bs_CB_R = new RooCBShape("Bs_CB_R", "", Bd_M, *Bs_mean, *Bs_sigma_R, *Bs_alpha_R, *Bs_n_R);
    RooAddPdf * Bs_shape = new RooAddPdf("Bs_shape", "", RooArgList(*Bs_CB_L, *Bs_CB_R), RooArgList(*Bs_frac));

    // =====================
    // Make rho mis-ID shape
    // =====================
    // Set up parameters
    RooRealVar * rho_mean = new RooRealVar("rho_mean", "", pr["rho_mean"]);
    RooRealVar * rho_sigma_L = new RooRealVar("rho_sigma_L", "", pr["rho_sigma_L"]);
    RooRealVar * rho_sigma_ratio = new RooRealVar("rho_sigma_ratio", "", pr["rho_sigma_ratio"]);
    RooFormulaVar * rho_sigma_R = new RooFormulaVar("rho_sigma_R", "@0 * @1", RooArgList(*rho_sigma_L, *rho_sigma_ratio));
    RooRealVar * rho_alpha_L = new RooRealVar("rho_alpha_L", "", pr["rho_alpha_L"]);
    RooRealVar * rho_alpha_R = new RooRealVar("rho_alpha_R", "", pr["rho_alpha_R"]);
    RooRealVar * rho_n_L = new RooRealVar("rho_n_L", "", pr["rho_n_L"]);
    RooRealVar * rho_n_R = new RooRealVar("rho_n_R", "", pr["rho_n_R"]);
    RooRealVar * rho_frac = new RooRealVar("rho_frac", "", pr["rho_frac"]);

    // Make crystal ball PDFs
    RooCBShape * rho_CB_L = new RooCBShape("rho_CB_L", "", Bd_M, *rho_mean, *rho_sigma_L, *rho_alpha_L, *rho_n_L);
    RooCBShape * rho_CB_R = new RooCBShape("rho_CB_R", "", Bd_M, *rho_mean, *rho_sigma_R, *rho_alpha_R, *rho_n_R);
    RooAddPdf * rho_shape = new RooAddPdf("rho_shape", "", RooArgList(*rho_CB_L, *rho_CB_R), RooArgList(*rho_frac));

    // ====================
    // Make low mass shapes
    // ====================
    // Kinematic endpoints
    RooRealVar * low_a_gamma = new RooRealVar("low_a_gamma", "", pr["low_a_gamma"]);
    RooRealVar * low_b_gamma = new RooRealVar("low_b_gamma", "", pr["low_b_gamma"]);
    RooRealVar * low_a_pi = new RooRealVar("low_a_pi", "", pr["low_a_pi"]);
    RooRealVar * low_b_pi = new RooRealVar("low_b_pi", "", pr["low_b_pi"]);

    // Widths
    RooRealVar * low_sigma_gamma_010 = new RooRealVar("low_sigma_gamma_010", "", pr["low_sigma_gamma_010"]);
    RooRealVar * low_sigma_gamma_101 = new RooRealVar("low_sigma_gamma_101", "", pr["low_sigma_gamma_101"]);
    RooRealVar * low_sigma_pi_010 = new RooRealVar("low_sigma_pi_010", "", pr["low_sigma_pi_010"]);
    RooRealVar * low_sigma_pi_101 = new RooRealVar("low_sigma_pi_101", "", pr["low_sigma_pi_101"]);

    // Csi
    RooRealVar * low_csi_gamma_010 = new RooRealVar("low_csi_gamma_010", "", pr["low_csi_gamma_010"]);
    RooRealVar * low_csi_gamma_101 = new RooRealVar("low_csi_gamma_101", "", pr["low_csi_gamma_101"]);
    RooRealVar * low_csi_pi_010 = new RooRealVar("low_csi_pi_010", "", pr["low_csi_pi_010"]);
    RooRealVar * low_csi_pi_101 = new RooRealVar("low_csi_pi_101", "", pr["low_csi_pi_101"]);

    // Fraction and ratio of Gaussians
    RooRealVar * low_frac = new RooRealVar("low_frac", "", pr["low_frac"]);
    RooRealVar * low_ratio = new RooRealVar("low_ratio", "", pr["low_ratio"]);
    RooRealVar * low_shiftg = new RooRealVar("low_shiftg", "", 0);

    // Make shapes
    RooHILLdini * gamma_010_shape = new RooHILLdini("gamma_010_shape", "", Bd_M, 
            *low_a_gamma, *low_b_gamma, *low_csi_gamma_010, *shift, 
            *low_sigma_gamma_010, *low_ratio, *low_frac);
    RooLITTLEHORNSdini * gamma_101_shape = new RooLITTLEHORNSdini("gamma_101_shape", "", Bd_M, 
            *low_a_gamma, *low_b_gamma, *low_csi_gamma_101, *shift, 
            *low_sigma_gamma_101, *low_ratio, *low_frac, *low_shiftg);
    RooHORNSdini * pi_010_shape = new RooHORNSdini("pi_010_shape", "", Bd_M, 
            *low_a_pi, *low_b_pi, *low_csi_pi_010, *shift, 
            *low_sigma_pi_010, *low_ratio, *low_frac);
    RooHILLdini * pi_101_shape = new RooHILLdini("pi_101_shape", "", Bd_M, 
            *low_a_pi, *low_b_pi, *low_csi_pi_101, *shift, 
            *low_sigma_pi_101, *low_ratio, *low_frac);

    // =======================
    // Make Bs low mass shapes
    // =======================
    // Shifted endpoints
    RooFormulaVar * Bs_low_a_gamma = new RooFormulaVar("Bs_low_a_gamma", "@0 + @1", RooArgList(*low_a_gamma, *delta_M));
    RooFormulaVar * Bs_low_b_gamma = new RooFormulaVar("Bs_low_b_gamma", "@0 + @1", RooArgList(*low_b_gamma, *delta_M));
    RooFormulaVar * Bs_low_a_pi = new RooFormulaVar("Bs_low_a_pi", "@0 + @1", RooArgList(*low_a_pi, *delta_M));
    RooFormulaVar * Bs_low_b_pi = new RooFormulaVar("Bs_low_b_pi", "@0 + @1", RooArgList(*low_b_pi, *delta_M));

    // Widths
    RooRealVar * Bs_low_sigma_gamma_010 = new RooRealVar("Bs_low_sigma_gamma_010", "", pr["low_Bs_sigma_gamma_010"]);
    RooRealVar * Bs_low_sigma_gamma_101 = new RooRealVar("Bs_low_sigma_gamma_101", "", pr["low_Bs_sigma_gamma_101"]);
    RooRealVar * Bs_low_sigma_pi_010 = new RooRealVar("Bs_low_sigma_pi_010", "", pr["low_Bs_sigma_pi_010"]);
    RooRealVar * Bs_low_sigma_pi_101 = new RooRealVar("Bs_low_sigma_pi_101", "", pr["low_Bs_sigma_pi_101"]);

    // Csi
    RooRealVar * Bs_low_csi_gamma_010 = new RooRealVar("Bs_low_csi_gamma_010", "", pr["low_Bs_csi_gamma_010"]);
    RooRealVar * Bs_low_csi_gamma_101 = new RooRealVar("Bs_low_csi_gamma_101", "", pr["low_Bs_csi_gamma_101"]);
    RooRealVar * Bs_low_csi_pi_010 = new RooRealVar("Bs_low_csi_pi_010", "", pr["low_Bs_csi_pi_010"]);
    RooRealVar * Bs_low_csi_pi_101 = new RooRealVar("Bs_low_csi_pi_101", "", pr["low_Bs_csi_pi_101"]);

    // Make shapes
    RooHILLdini * Bs_gamma_010_shape = new RooHILLdini("Bs_gamma_010_shape", "", Bd_M, 
            *Bs_low_a_gamma, *Bs_low_b_gamma, *Bs_low_csi_gamma_010, *shift, 
            *Bs_low_sigma_gamma_010, *low_ratio, *low_frac);
    RooLITTLEHORNSdini * Bs_gamma_101_shape = new RooLITTLEHORNSdini("Bs_gamma_101_shape", "", Bd_M, 
            *Bs_low_a_gamma, *Bs_low_b_gamma, *Bs_low_csi_gamma_101, *shift, 
            *Bs_low_sigma_gamma_101, *low_ratio, *low_frac, *shift);
    RooHORNSdini * Bs_pi_010_shape = new RooHORNSdini("Bs_pi_010_shape", "", Bd_M, 
            *Bs_low_a_pi, *Bs_low_b_pi, *Bs_low_csi_pi_010, *shift, 
            *Bs_low_sigma_pi_010, *low_ratio, *low_frac);
    RooHILLdini * Bs_pi_101_shape = new RooHILLdini("Bs_pi_101_shape", "", Bd_M, 
            *Bs_low_a_pi, *Bs_low_b_pi, *Bs_low_csi_pi_101, *shift, 
            *Bs_low_sigma_pi_101, *low_ratio, *low_frac);

    // =======================
    // Combine low mass shapes
    // =======================
    // Calculate fractions of pi and gamma
    const double branching_gamma = 0.353;
    const double branching_pi = 0.647;
    const double acc_gamma = 17.48;
    const double acc_pi = 15.53;
    double G_010 = branching_gamma * acc_gamma * 0.009227;
    double G_101 = branching_gamma * acc_gamma * 0.004613; 
    double P_010 = branching_pi * acc_pi * 0.01168;
    double P_101 = branching_pi * acc_pi * 0.005712;
    
    // Make helicity fraction parameters
    RooRealVar * coeff_gamma_010 = new RooRealVar("coeff_gamma_010", "", G_010 / (P_010 + G_010));
    RooRealVar * coeff_gamma_101 = new RooRealVar("coeff_gamma_101", "", G_101 / (P_101 + G_101));
    RooRealVar * coeff_pi_010 = new RooRealVar("coeff_pi_010", "", P_010 / (P_010 + G_010));
    RooRealVar * coeff_pi_101 = new RooRealVar("coeff_pi_101", "", P_101 / (P_101 + G_101));

    // Combine helicity components
    RooAddPdf * low_010_shape = new RooAddPdf("low_010_shape", "", 
            RooArgList(*gamma_010_shape, *pi_010_shape), 
            RooArgList(*coeff_gamma_010, *coeff_pi_010));
    RooAddPdf * low_101_shape = new RooAddPdf("low_101_shape", "", 
            RooArgList(*gamma_101_shape, *pi_101_shape), 
            RooArgList(*coeff_gamma_101, *coeff_pi_101));
    RooAddPdf * Bs_low_010_shape = new RooAddPdf("Bs_low_010_shape", "", 
            RooArgList(*Bs_gamma_010_shape, *Bs_pi_010_shape), 
            RooArgList(*coeff_gamma_010, *coeff_pi_010));
    RooAddPdf * Bs_low_101_shape = new RooAddPdf("Bs_low_101_shape", "", 
            RooArgList(*Bs_gamma_101_shape, *Bs_pi_101_shape), 
            RooArgList(*coeff_gamma_101, *coeff_pi_101));

    // Make floating fraction of 010 shape
    RooRealVar * low_frac_010_Kpipipi = new RooRealVar("low_frac_010_Kpipipi", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_piKpipi = new RooRealVar("low_frac_010_piKpipi", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_piKpipi_plus = new RooRealVar("low_frac_010_piKpipi_plus", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_piKpipi_minus = new RooRealVar("low_frac_010_piKpipi_minus", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_pipipipi = new RooRealVar("low_frac_010_pipipipi", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_pipipipi_plus = new RooRealVar("low_frac_010_pipipipi_plus", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_pipipipi_minus = new RooRealVar("low_frac_010_pipipipi_minus", "", 0.5, 0, 1);
    RooRealVar * Bs_low_frac_010 = new RooRealVar("Bs_low_frac_010", "", 0.5, 0, 1);

    // Combine helicity components for combined fit
    std::map<std::string, RooAddPdf*> low_shapes;
    std::map<std::string, RooAddPdf*> low_shapes_plus;
    std::map<std::string, RooAddPdf*> low_shapes_minus;
    low_shapes["Kpipipi"] = new RooAddPdf("low_Kpipipi_shape", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_Kpipipi));
    low_shapes["piKpipi"] = new RooAddPdf("low_piKpipi_shape", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_piKpipi));
    low_shapes["pipipipi"] = new RooAddPdf("low_pipipipi_shape", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_pipipipi));
    RooAddPdf * Bs_low_shape = new RooAddPdf("Bs_low_shape", "", 
            RooArgList(*Bs_low_010_shape, *Bs_low_101_shape), RooArgList(*Bs_low_frac_010));

    // Separate shapes for flavour split fit to account for CP violation
    low_shapes_plus["Kpipipi"] = low_shapes["Kpipipi"];
    low_shapes_minus["Kpipipi"] = low_shapes["Kpipipi"];
    low_shapes_plus["piKpipi"] = new RooAddPdf("low_piKpipi_shape_plus", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_piKpipi_plus));
    low_shapes_minus["piKpipi"] = new RooAddPdf("low_piKpipi_shape_minus", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_piKpipi_minus));
    low_shapes_plus["pipipipi"] = new RooAddPdf("low_pipipipi_shape_plus", "", RooArgList(*low_010_shape, *low_101_shape),
            RooArgList(*low_frac_010_pipipipi_plus));
    low_shapes_minus["pipipipi"] = new RooAddPdf("low_pipipipi_shape_minus", "", RooArgList(*low_010_shape, *low_101_shape),
            RooArgList(*low_frac_010_pipipipi_minus));

    // ========================
    // Make combinatorial shape
    // ========================
    std::map<std::string, RooExponential *> expo_shapes;
    RooRealVar * slope_Kpipipi = new RooRealVar("slope_Kpipipi", "", -0.005, -0.5, 0.0);
    RooRealVar * slope_pipipipi = new RooRealVar("slope_pipipipi", "", -0.005, -0.5, 0.0);
    expo_shapes["Kpipipi"] = new RooExponential("expo_Kpipipi_shape", "", Bd_M, *slope_Kpipipi);
    expo_shapes["piKpipi"] = expo_shapes["Kpipipi"];
    expo_shapes["pipipipi"] = new RooExponential("expo_pipipipi_shape", "", Bd_M, *slope_pipipipi);

    // ==================
    // Set up observables
    // ==================
    // Signal asymmetries; a_Kpipipi is ratio n_minus / n_plus
    RooRealVar * A_Kpipipi = new RooRealVar("A_Kpipipi", "", 0, -1, 1);
    RooFormulaVar * a_Kpipipi = new RooFormulaVar("a_Kpipipi", "(1 + @0) / (1 - @0)", RooArgList(*A_Kpipipi));

    // Pipipipi blind asymmetries
    RooRealVar * A_pipipipi_blind = new RooRealVar("A_pipipipi_blind", "", 0, -1, 1);
    RooUnblindUniform * A_pipipipi = new RooUnblindUniform("A_pipipipi", "", "blind_pipipipi_asym", 0.1, *A_pipipipi_blind);
    RooFormulaVar * a_pipipipi = new RooFormulaVar("a_pipipipi", "(1 + @0) / (1 - @0)", RooArgList(*A_pipipipi));

    // Bs asymmetries (check these for signs etc later)
    RooRealVar * A_piKpipi_Bs = new RooRealVar("A_piKpipi_Bs", "", 0, -1, 1);
    RooRealVar * A_pipipipi_Bs = new RooRealVar("A_pipipipi_Bs", "", 0, -1, 1);
    RooFormulaVar * a_piKpipi_Bs = new RooFormulaVar("a_piKpipi_Bs", "(1 + @0) / (1 - @0)", RooArgList(*A_piKpipi_Bs));
    RooFormulaVar * a_pipipipi_Bs = new RooFormulaVar("a_pipipipi_Bs", "(1 + @0) / (1 - @0)", RooArgList(*A_pipipipi_Bs));

    // Low mass asymmetries
    RooRealVar * A_Kpipipi_low = new RooRealVar("A_Kpipipi_low", "", 0, -1, 1);
    RooRealVar * A_pipipipi_low = new RooRealVar("A_pipipipi_low", "", 0, -1, 1);
    RooFormulaVar * a_Kpipipi_low = new RooFormulaVar("a_Kpipipi_low", "(1 + @0) / (1 - @0)", RooArgList(*A_Kpipipi_low));
    RooFormulaVar * a_pipipipi_low = new RooFormulaVar("a_pipipipi_low", "(1 + @0) / (1 - @0)", RooArgList(*A_pipipipi_low));

    // Bs low mass asymmetries
    RooRealVar * A_piKpipi_Bs_low = new RooRealVar("A_piKpipi_Bs_low", "", 0, -1, 1);
    RooRealVar * A_pipipipi_Bs_low = new RooRealVar("A_pipipipi_Bs_low", "", 0, -1, 1);
    RooFormulaVar * a_piKpipi_Bs_low = new RooFormulaVar("a_piKpipi_Bs_low", "(1 + @0) / (1 - @0)", RooArgList(*A_piKpipi_Bs_low));
    RooFormulaVar * a_pipipipi_Bs_low = new RooFormulaVar("a_pipipipi_Bs_low", "(1 + @0) / (1 - @0)", RooArgList(*A_pipipipi_Bs_low));

    // Signal ratios
    // Suppressed to favoured ADS ratio
    RooRealVar * R_piKpipi_vs_Kpipipi_blind = new RooRealVar("R_piKpipi_vs_Kpipipi_blind", "", 0.06, 0, 1);
    RooUnblindUniform * R_piKpipi_vs_Kpipipi = new RooUnblindUniform("R_piKpipi_vs_Kpipipi", "", "blind_piKpipi_ratio", 0.01, *R_piKpipi_vs_Kpipipi_blind);

    // CP modes to favoured ADS ratio
    RooRealVar * R_pipipipi_vs_Kpipipi_blind = new RooRealVar("R_pipipipi_vs_Kpipipi_blind", "", 0.1, 0, 1);
    RooUnblindUniform * R_pipipipi_vs_Kpipipi = new RooUnblindUniform("R_pipipipi_vs_Kpipipi", "", "blind_pipipipi_ratio", 0.05, *R_pipipipi_vs_Kpipipi_blind);

    // Flavour split ratios
    RooRealVar * R_plus_blind = new RooRealVar("R_plus_blind", "", 0.06, 0, 1);
    RooRealVar * R_minus_blind = new RooRealVar("R_minus_blind", "", 0.06, 0, 1);
    RooUnblindUniform * R_plus = new RooUnblindUniform("R_plus", "", "blind_R_plus", 0.01, *R_plus_blind);
    RooUnblindUniform * R_minus = new RooUnblindUniform("R_minus", "", "blind_R_minus", 0.01, *R_minus_blind);

    // Bs ratios
    RooRealVar * R_pipipipi_vs_piKpipi_Bs = new RooRealVar("R_pipipipi_vs_piKpipi_Bs", "", 0.1, 0, 1);

    // Low mass ratios
    RooRealVar * R_piKpipi_vs_Kpipipi_low = new RooRealVar("R_piKpipi_vs_Kpipipi_low", "", 0.06, 0, 1);
    RooRealVar * R_pipipipi_vs_Kpipipi_low = new RooRealVar("R_pipipipi_vs_Kpipipi_low", "", 0.1, 0, 1);

    // Low mass flavour split ratios
    RooRealVar * R_plus_low = new RooRealVar("R_plus_low", "", 0.06, 0, 1);
    RooRealVar * R_minus_low = new RooRealVar("R_minus_low", "", 0.06, 0, 1);

    // Low mass Bs ratios
    RooRealVar * R_pipipipi_vs_piKpipi_Bs_low = new RooRealVar("R_pipipipi_vs_piKpipi_Bs_low", "", 0.1, 0, 1);

    // =============
    // Set up yields
    // =============
    // Maps to hold yields for each mode
    std::map<std::string, std::map<std::string, RooAbsReal *>> yields;
    std::map<std::string, std::map<std::string, RooAbsReal *>> yields_plus;
    std::map<std::string, std::map<std::string, RooAbsReal *>> yields_minus;

    // Total signal yields
    yields["Kpipipi"]["n_signal"] = new RooRealVar("n_signal_Kpipipi", "", 100, 0, 20000);
    yields["piKpipi"]["n_signal"] = new RooFormulaVar("n_signal_piKpipi", "@0 * @1", 
            RooArgList(*yields["Kpipipi"]["n_signal"], *R_piKpipi_vs_Kpipipi));
    yields["pipipipi"]["n_signal"] = new RooFormulaVar("n_signal_pipipipi", "@0 * @1", RooArgList(*yields["Kpipipi"]["n_signal"], *R_pipipipi_vs_Kpipipi));

    // Flavour split signal yields
    yields_plus["Kpipipi"]["n_signal"] = new RooFormulaVar("n_signal_Kpipipi_plus", "@0 / (1 + @1)", RooArgList(*yields["Kpipipi"]["n_signal"], *a_Kpipipi)); 
    yields_minus["Kpipipi"]["n_signal"] = new RooFormulaVar("n_signal_Kpipipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["Kpipipi"]["n_signal"], *a_Kpipipi)); 
    yields_plus["pipipipi"]["n_signal"] = new RooFormulaVar("n_signal_pipipipi_plus", "@0 / (1 + @1)", RooArgList(*yields["pipipipi"]["n_signal"], *a_pipipipi)); 
    yields_minus["pipipipi"]["n_signal"] = new RooFormulaVar("n_signal_pipipipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["pipipipi"]["n_signal"], *a_pipipipi)); 

    // Make suppressed mode yields from CP ratios
    yields_plus["piKpipi"]["n_signal"] = new RooFormulaVar("n_signal_piKpipi_plus", "@0 * @1", RooArgList(*yields_plus["Kpipipi"]["n_signal"], *R_plus));
    yields_minus["piKpipi"]["n_signal"] = new RooFormulaVar("n_signal_piKpipi_minus", "@0 * @1", RooArgList(*yields_minus["Kpipipi"]["n_signal"], *R_minus));

    // Bs yields
    yields["piKpipi"]["n_Bs"] = new RooRealVar("n_Bs_piKpipi", "", 100, 0, 20000);
    yields["pipipipi"]["n_Bs"] = new RooFormulaVar("n_Bs_pipipipi", "@0 * @1", RooArgList(*yields["piKpipi"]["n_Bs"], *R_pipipipi_vs_piKpipi_Bs));

    // Flavour split Bs yields
    yields_plus["piKpipi"]["n_Bs"] = new RooFormulaVar("n_Bs_piKpipi_plus", "@0 / (1 + @1)", RooArgList(*yields["piKpipi"]["n_Bs"], *a_piKpipi_Bs)); 
    yields_minus["piKpipi"]["n_Bs"] = new RooFormulaVar("n_Bs_piKpipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["piKpipi"]["n_Bs"], *a_piKpipi_Bs)); 
    yields_plus["pipipipi"]["n_Bs"] = new RooFormulaVar("n_Bs_pipipipi_plus", "@0 / (1 + @1)", RooArgList(*yields["pipipipi"]["n_Bs"], *a_pipipipi_Bs)); 
    yields_minus["pipipipi"]["n_Bs"] = new RooFormulaVar("n_Bs_pipipipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["pipipipi"]["n_Bs"], *a_pipipipi_Bs)); 

    // Low mass yields
    yields["Kpipipi"]["n_low"] = new RooRealVar("n_low_Kpipipi", "", 100, 0, 20000);
    yields["piKpipi"]["n_low"] = new RooFormulaVar("n_low_piKpipi", "@0 * @1", RooArgList(*yields["Kpipipi"]["n_low"], *R_piKpipi_vs_Kpipipi_low));
    yields["pipipipi"]["n_low"] = new RooFormulaVar("n_low_pipipipi", "@0 * @1", RooArgList(*yields["Kpipipi"]["n_low"], *R_pipipipi_vs_Kpipipi_low));

    // Flavour split low mass yields
    yields_plus["Kpipipi"]["n_low"] = new RooFormulaVar("n_low_Kpipipi_plus", "@0 / (1 + @1)", RooArgList(*yields["Kpipipi"]["n_low"], *a_Kpipipi_low));
    yields_minus["Kpipipi"]["n_low"] = new RooFormulaVar("n_low_Kpipipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["Kpipipi"]["n_low"], *a_Kpipipi_low));
    yields_plus["pipipipi"]["n_low"] = new RooFormulaVar("n_low_pipipipi_plus", "@0 / (1 + @1)", RooArgList(*yields["pipipipi"]["n_low"], *a_pipipipi_low));
    yields_minus["pipipipi"]["n_low"] = new RooFormulaVar("n_low_pipipipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["pipipipi"]["n_low"], *a_pipipipi_low));

    // Split piK low mass yields using ratios
    yields_plus["piKpipi"]["n_low"] = new RooFormulaVar("n_low_piKpipi_plus", "@0 * @1", RooArgList(*yields_plus["Kpipipi"]["n_low"], *R_plus_low));
    yields_minus["piKpipi"]["n_low"] = new RooFormulaVar("n_low_piKpipi_minus", "@0 * @1", RooArgList(*yields_minus["Kpipipi"]["n_low"], *R_minus_low));

    // Bs low mass yields
    yields["piKpipi"]["n_Bs_low"] = new RooRealVar("n_Bs_low_piKpipi", "", 100, 0, 20000);
    yields["pipipipi"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_pipipipi", "@0 * @1", RooArgList(*yields["piKpipi"]["n_Bs_low"], *R_pipipipi_vs_piKpipi_Bs_low));

    // Flavour split Bs low mass yields
    yields_plus["piKpipi"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_piKpipi_plus", "@0 / (1 + @1)", RooArgList(*yields["piKpipi"]["n_Bs_low"], *a_piKpipi_Bs_low));
    yields_minus["piKpipi"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_piKpipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["piKpipi"]["n_Bs_low"], *a_piKpipi_Bs_low));
    yields_plus["pipipipi"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_pipipipi_plus", "@0 / (1 + @1)", RooArgList(*yields["pipipipi"]["n_Bs_low"], *a_pipipipi_Bs_low));
    yields_minus["pipipipi"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_pipipipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["pipipipi"]["n_Bs_low"], *a_pipipipi_Bs_low));

    // Rho yields (shared between piK and Kpi)
    yields["Kpipipi"]["n_rho"] = new RooRealVar("n_rho_Kpipipi", "", 50, 0, 1000);
    yields["piKpipi"]["n_rho"] = yields["Kpipipi"]["n_rho"];
    yields["pipipipi"]["n_rho"] = new RooFormulaVar("n_rho_pipipipi", "@0 * @1", RooArgList(*yields["piKpipi"]["n_rho"], *R_pipipipi_vs_piKpipi_Bs));

    // Flavour split rho yields (split equally)
    yields_plus["Kpipipi"]["n_rho"] = new RooFormulaVar("n_rho_Kpipipi_plus", "@0 / 2", RooArgList(*yields["Kpipipi"]["n_rho"]));
    yields_minus["Kpipipi"]["n_rho"] = new RooFormulaVar("n_rho_Kpipipi_minus", "@0 / 2", RooArgList(*yields["Kpipipi"]["n_rho"]));
    yields_plus["piKpipi"]["n_rho"] = new RooFormulaVar("n_rho_piKpipi_plus", "@0 / 2", RooArgList(*yields["piKpipi"]["n_rho"]));
    yields_minus["piKpipi"]["n_rho"] = new RooFormulaVar("n_rho_piKpipi_minus", "@0 / 2", RooArgList(*yields["piKpipi"]["n_rho"]));
    yields_plus["pipipipi"]["n_rho"] = new RooFormulaVar("n_rho_pipipipi_plus", "@0 / 2", RooArgList(*yields["pipipipi"]["n_rho"]));
    // Exponential yields
    yields["Kpipipi"]["n_expo"] = new RooRealVar("n_expo_Kpipipi", "", 100, 0, 20000);
    yields["piKpipi"]["n_expo"] = new RooRealVar("n_expo_piKpipi", "", 100, 0, 20000);
    yields["pipipipi"]["n_expo"] = new RooRealVar("n_expo_pipipipi", "", 100, 0, 20000);

    // Flavour split exponential yields (split equally)
    yields_plus["Kpipipi"]["n_expo"] = new RooFormulaVar("n_expo_Kpipipi_plus", "@0 / 2", RooArgList(*yields["Kpipipi"]["n_expo"]));
    yields_minus["Kpipipi"]["n_expo"] = new RooFormulaVar("n_expo_Kpipipi_minus", "@0 / 2", RooArgList(*yields["Kpipipi"]["n_expo"]));
    yields_plus["piKpipi"]["n_expo"] = new RooFormulaVar("n_expo_piKpipi_plus", "@0 / 2", RooArgList(*yields["piKpipi"]["n_expo"]));
    yields_minus["piKpipi"]["n_expo"] = new RooFormulaVar("n_expo_piKpipi_minus", "@0 / 2", RooArgList(*yields["piKpipi"]["n_expo"]));
    yields_plus["pipipipi"]["n_expo"] = new RooFormulaVar("n_expo_pipipipi_plus", "@0 / 2", RooArgList(*yields["pipipipi"]["n_expo"]));
    yields_minus["pipipipi"]["n_expo"] = new RooFormulaVar("n_expo_pipipipi_minus", "@0 / 2", RooArgList(*yields["pipipipi"]["n_expo"]));

    // ======================
    // Make overall fit shape
    // ======================
    // Set up categories
    RooCategory category("category", "");
    for (auto mode : modes) {
        if (sum == "Y") {
            category.defineType(mode.c_str());
        } else {
            category.defineType((mode + "_plus").c_str());
            category.defineType((mode + "_minus").c_str());
        }
    }

    // Make combined dataset
    RooAbsData * combData;
    if (binned == "Y") {

        // Binned fit: convert to RooDataHist
        std::map<std::string, RooDataHist *> all_data;
        for (auto mode : modes) {
            if (sum == "Y") {
                all_data[mode] = data_both[mode]->binnedClone();
            } else {
                all_data[mode + "_plus"] = data_plus[mode]->binnedClone();
                all_data[mode + "_minus"] = data_minus[mode]->binnedClone();
            }
        }
        combData = new RooDataHist("combData", "", Bd_M, category, all_data);
    } else {

        // Unbinned fit: keep as RooDataSet
        std::map<std::string, RooDataSet *> all_data;
        for (auto mode : modes) {
            if (sum == "Y") {
                all_data[mode] = data_both[mode];
            } else {
                all_data[mode + "_plus"] = data_plus[mode];
                all_data[mode + "_minus"] = data_minus[mode];
            }
        }
        combData = new RooDataSet("combData", "", Bd_M, RooFit::Index(category), 
               RooFit::Import(all_data));
    }

    // Make each total fit shape
    std::map<std::string, RooAddPdf *> fitShapes;
    for (auto mode : modes) {
        if (sum == "Y") {
            // Lists of shapes and yields
            RooArgList shapes_list;
            RooArgList yields_list;

            // Add shapes
            shapes_list.add(*signal_shape);
            shapes_list.add(*(expo_shapes[mode]));
            shapes_list.add(*(low_shapes[mode]));
            shapes_list.add(*rho_shape);
            if (mode != "Kpipipi") {
                shapes_list.add(*Bs_shape);
                shapes_list.add(*Bs_low_shape);
            }

            // Add yields
            yields_list.add(*(yields[mode]["n_signal"]));
            yields_list.add(*(yields[mode]["n_expo"]));
            yields_list.add(*(yields[mode]["n_low"]));
            yields_list.add(*(yields[mode]["n_rho"]));
            if (mode != "Kpipipi") {
                yields_list.add(*(yields[mode]["n_Bs"]));
                yields_list.add(*(yields[mode]["n_Bs_low"]));
            }

            // Make shape
            fitShapes[mode] = new RooAddPdf(("fit_" + mode).c_str(), "", shapes_list, yields_list);

        } else {

            // List of shapes and yields
            std::cout << "Making fit shape for " << mode << std::endl;
            RooArgList shapes_list_plus;
            RooArgList shapes_list_minus;
            RooArgList yields_list_plus;
            RooArgList yields_list_minus;

            // Add shapes
            shapes_list_plus.add(*signal_shape);
            shapes_list_minus.add(*signal_shape);
            shapes_list_plus.add(*(expo_shapes[mode]));
            shapes_list_minus.add(*(expo_shapes[mode]));
            shapes_list_plus.add(*(low_shapes_plus[mode]));
            shapes_list_minus.add(*(low_shapes_minus[mode]));
            shapes_list_plus.add(*rho_shape);
            shapes_list_minus.add(*rho_shape);
            if (mode != "Kpipipi") {
                shapes_list_plus.add(*Bs_shape);
                shapes_list_minus.add(*Bs_shape);
                shapes_list_plus.add(*Bs_low_shape);
                shapes_list_minus.add(*Bs_low_shape);
            }

            // Add yields
            yields_list_plus.add(*(yields_plus[mode]["n_signal"]));
            yields_list_minus.add(*(yields_minus[mode]["n_signal"]));
            yields_list_plus.add(*(yields_plus[mode]["n_expo"]));
            yields_list_minus.add(*(yields_minus[mode]["n_expo"]));
            yields_list_plus.add(*(yields_plus[mode]["n_low"]));
            yields_list_minus.add(*(yields_minus[mode]["n_low"]));
            yields_list_plus.add(*(yields_plus[mode]["n_rho"]));
            yields_list_minus.add(*(yields_minus[mode]["n_rho"]));
            if (mode != "Kpipipi") {
                yields_list_plus.add(*(yields_plus[mode]["n_Bs"]));
                yields_list_minus.add(*(yields_minus[mode]["n_Bs"]));
                yields_list_plus.add(*(yields_plus[mode]["n_Bs_low"]));
                yields_list_minus.add(*(yields_minus[mode]["n_Bs_low"]));
            }

            // Print lists
            yields_list_plus.Print();
            yields_list_minus.Print();
            shapes_list_plus.Print();
            shapes_list_minus.Print();

            // Make shapes
            fitShapes[mode + "_plus"] = new RooAddPdf(("fit_" + mode + "_plus").c_str(), "", shapes_list_plus, yields_list_plus);
            fitShapes[mode + "_minus"] = new RooAddPdf(("fit_" + mode + "_minus").c_str(), "", shapes_list_minus, yields_list_minus);
        }
    }

    // Combine the PDFs
    RooSimultaneous * simPdf = new RooSimultaneous("simPdf", "", category);
    for (auto mode : modes) {
        if (sum == "Y") {
            simPdf->addPdf(*(fitShapes[mode]), mode.c_str());
        } else {
            simPdf->addPdf(*(fitShapes[mode + "_plus"]), (mode + "_plus").c_str());
            simPdf->addPdf(*(fitShapes[mode + "_minus"]), (mode + "_minus").c_str());
        }
    }

    // ===========
    // Perform fit
    // ===========
    RooFitResult * r;
    if (binned == "Y") {
        r = simPdf->fitTo(*((RooDataHist*)combData), RooFit::Save(), RooFit::NumCPU(8, 2),
                RooFit::Optimize(false), RooFit::Offset(true), 
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    } else {
        r = simPdf->fitTo(*((RooDataSet*)combData), RooFit::Save(), RooFit::NumCPU(8, 2),
                RooFit::Optimize(false), RooFit::Offset(true), 
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    }
    r->Print("v");
    std::string sum_string = ((sum == "Y") ? "combined" : "split");
    std::string bin_string = ((binned == "Y") ? "binned" : "unbinned");
    TFile * result_file = TFile::Open(("../Results/fourBody_" + input_year + "_" + 
                sum_string + "_" + bin_string + ".root").c_str(), "RECREATE");
    result_file->cd();
    r->Write();
    result_file->Close();

    // ================================
    // Save results to a histogram file
    // ================================
    // Open file
    std::string outfile_name;
    if (sum == "Y") { 
        outfile_name = "../Histograms/fits_fourBody_combined_" + bin_string + ".root";
    } else {
        outfile_name = "../Histograms/fits_fourBody_split_" + bin_string + ".root";
    }
    TFile * outfile = TFile::Open(outfile_name.c_str(), "RECREATE");

    // Loop through modes
    for (auto mode : modes) {

        // Categories to use for each mode
        std::vector<std::string> mode_categories;
        if (sum == "Y") {
            mode_categories.push_back("both");
        } else {
            mode_categories.push_back("plus");
            mode_categories.push_back("minus");
        }

        // Loop through categories
        for (auto cat : mode_categories) {

            // Name to save histograms under
            std::string fullname = mode;
            if (cat != "both") fullname = mode + "_" + cat;
            std::cout << "Saving histograms for: " << fullname << std::endl;

            // Data and yields to use
            std::map<std::string, RooDataSet *> * dataMap;
            std::map<std::string, RooAddPdf *> * lowMap;
            std::map<std::string, std::map<std::string, RooAbsReal *>> * yieldMap;
            if (cat == "both") {
                dataMap = &data_both;
                lowMap = &low_shapes;
                yieldMap = &yields;
            } else if (cat == "plus") {
                dataMap = &data_plus;
                lowMap = &low_shapes_plus;
                yieldMap = &yields_plus;
            } else if (cat == "minus") {
                dataMap = &data_minus;
                lowMap = &low_shapes_minus;
                yieldMap = &yields_minus;
            } 

            // Convert data and fit to TH1Fs
            std::cout << "Converting data and fit" << std::endl;
            TH1F * h_data = (TH1F*)(*dataMap)[mode]->createHistogram(("h_data_" + fullname).c_str(), Bd_M);
            TH1F * h_fit = (TH1F*)fitShapes[fullname]->createHistogram(("h_fit_" + fullname).c_str(), Bd_M, RooFit::Binning(nBins * 10));

            // Convert fit components
            std::cout << "Converting fit components" << std::endl;
            TH1F * h_signal = (TH1F*)signal_shape->createHistogram(("h_signal_" + fullname).c_str(), Bd_M, RooFit::Binning(nBins * 10));
            TH1F * h_expo = (TH1F*)expo_shapes[mode]->createHistogram(("h_expo_" + fullname).c_str(), Bd_M, RooFit::Binning(nBins * 10));
            TH1F * h_low = (TH1F*)(*lowMap)[mode]->createHistogram(("h_low_" + fullname).c_str(), Bd_M, RooFit::Binning(nBins * 10));
            TH1F * h_rho = (TH1F*)rho_shape->createHistogram(("h_rho_" + fullname).c_str(), Bd_M, RooFit::Binning(nBins * 10));

            // Scale histograms
            std::cout << "Scaling histograms" << std::endl;
            h_fit->Scale(h_data->Integral() * 10 / h_fit->Integral());
            h_signal->Scale((*yieldMap)[mode]["n_signal"]->getVal() * 10 / h_signal->Integral());
            h_expo->Scale((*yieldMap)[mode]["n_expo"]->getVal() * 10 / h_expo->Integral());
            h_low->Scale((*yieldMap)[mode]["n_low"]->getVal() * 10 / h_low->Integral());
            h_rho->Scale((*yieldMap)[mode]["n_rho"]->getVal() * 10 / h_rho->Integral());

            // Save unblinded histos for Kpi
            if (mode == "Kpipipi") {

                // Save histograms
                outfile->cd();
                h_data->Write(("data_" + fullname).c_str());
                h_fit->Write(("fit_" + fullname).c_str());
                h_signal->Write(("signal_" + fullname).c_str());
                h_expo->Write(("expo_" + fullname).c_str());
                h_low->Write(("low_" + fullname).c_str());
                h_rho->Write(("rho_" + fullname).c_str());

                // Save pulls
                RooPlot * frame = Bd_M.frame();
                (*dataMap)[mode]->plotOn(frame);
                fitShapes[fullname]->plotOn(frame);
                RooHist * pulls = frame->pullHist();
                pulls->Write(("pulls_" + fullname).c_str());

            } else {

                // Make Bs histograms
                TH1F * h_Bs = (TH1F*)Bs_shape->createHistogram(("h_Bs_" + fullname).c_str(), Bd_M, RooFit::Binning(nBins * 10));
                TH1F * h_Bs_low = (TH1F*)Bs_low_shape->createHistogram(("h_Bs_low_" + fullname).c_str(), Bd_M, 
                        RooFit::Binning(nBins * 10));

                // Scale Bs histograms
                h_Bs->Scale((*yieldMap)[mode]["n_Bs"]->getVal() * 10 / h_Bs->Integral());
                h_Bs_low->Scale((*yieldMap)[mode]["n_Bs_low"]->getVal() * 10 / h_Bs_low->Integral());

                // Make blind histograms
                const double lsb = 5279.61 - 50;
                const double usb = 5279.61 + 50;
                TH1F * h_data_blind = h_data;
                TH1F * h_fit_blind = h_fit;
                TH1F * h_signal_blind = h_signal;
                TH1F * h_expo_blind = h_expo;
                TH1F * h_low_blind = h_low;
                TH1F * h_Bs_blind = h_Bs;
                TH1F * h_Bs_low_blind = h_Bs_low;
                TH1F * h_rho_blind = h_rho;

                // Remove data from blind region
                for (int bin = 1; bin < nBins; bin++) {
                    double bin_val = h_data->GetBinCenter(bin);
                    if (bin_val > lsb && bin_val < usb) h_data_blind->SetBinContent(bin, 0);
                }

                // Remove fits from blind region
                for (int bin = 1; bin < nBins * 10; bin++) {
                    double bin_val = h_fit->GetBinCenter(bin);
                    if (bin_val > lsb && bin_val < usb) {
                        h_fit_blind->SetBinContent(bin, 0);
                        h_signal_blind->SetBinContent(bin, 0);
                        h_expo_blind->SetBinContent(bin, 0);
                        h_low_blind->SetBinContent(bin, 0);
                        h_Bs_blind->SetBinContent(bin, 0);
                        h_Bs_low_blind->SetBinContent(bin, 0);
                        h_rho_blind->SetBinContent(bin, 0);
                    }
                }

                // Save blind histograms to file
                outfile->cd();
                h_data_blind->Write(("data_" + fullname).c_str());
                h_fit_blind->Write(("fit_" + fullname).c_str());
                h_signal_blind->Write(("signal_" + fullname).c_str());
                h_expo_blind->Write(("expo_" + fullname).c_str());
                h_low_blind->Write(("low_" + fullname).c_str());
                h_Bs_blind->Write(("Bs_" + fullname).c_str());
                h_Bs_low_blind->Write(("Bs_low_" + fullname).c_str());
                h_rho_blind->Write(("rho_" + fullname).c_str());

                // Save pulls
                RooPlot * frame = Bd_M.frame();
                (*dataMap)[mode]->plotOn(frame);
                fitShapes[fullname]->plotOn(frame);
                RooHist * pulls = frame->pullHist();
                pulls->Write(("pulls_" + fullname).c_str());
            }
        }
    }
    outfile->Close();

    // ================
    // Plot the results
    // ================
    Plotter * plotter = new Plotter();
    if (sum == "Y") {
        plotter->plotFourBodyFitsCombined(("../Histograms/fits_fourBody_combined_" + bin_string + ".root").c_str(), 
                "fourBody_" + input_year + "_" + bin_string, "");
    } else {
        plotter->plotFourBodyFitsSeparate(("../Histograms/fits_fourBody_split_" + bin_string + ".root").c_str(), 
                "fourBody_" + input_year + "_" + bin_string, "");
    }
    delete plotter;


    return 0;
}