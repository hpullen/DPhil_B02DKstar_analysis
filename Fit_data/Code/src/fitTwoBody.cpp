#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

#include "TApplication.h"
#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TRandom3.h"

#include "RooMCStudy.h"
#include "RooAddPdf.h"
#include "RooRandom.h"
#include "RooArgList.h"
#include "RooAbsData.h"
#include "RooDataHist.h"
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
    if (argc != 5 && argc != 9 && argc != 10) {
        std::cout << "Usage: ./FitTwoBody <2011:2012:2015:2016> <Sum: Y/N>"
            " <Binned: Y/N> <Toys: Y/N> (Custom BDT cuts: <Kpi-cut> <piK-cut> <KK-cut> <pipi-cut> <Save-plots: Y/N>)" << std::endl;
        return -1;
    }

    // Year (colon separated string)
    std::string input_year = argv[1];

    // Sum over charge (Y/N)
    std::string sum = argv[2];

    // Choice of binned/unbinned
    std::string binned = argv[3];

    // Whether or not to run toys
    std::string toys = argv[4];

    // Custom BDT cut if using
    bool custom_cuts = false;
    double cut_Kpi = 0.5;
    double cut_piK = 0.5;
    double cut_KK = 0.5;
    double cut_pipi = 0.5;
    std::string save_plots = "Y";
    if (argc >= 9) {
        custom_cuts = true;
        save_plots = "N";
        cut_Kpi = atof(argv[5]);
        cut_piK = atof(argv[6]);
        cut_KK = atof(argv[7]);
        cut_pipi = atof(argv[8]);
        if (argc == 10) save_plots = argv[9];
    }
    if (toys == "Y") save_plots = "N";

    // Vectors of years and D0 modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi"};

    // ================
    // Set up variables
    // ================
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5000, 5800);
    RooRealVar KstarK_ID("KstarK_ID", "", -100000, 100000);
    RooRealVar BDT_Kpi("BDTG_Kpi_run2", "", cut_Kpi, 1);
    RooRealVar BDT_piK("BDTG_Kpi_run2", "", cut_piK, 1);
    RooRealVar BDT_KK("BDTG_KK_run2", "", cut_KK, 1);
    RooRealVar BDT_pipi("BDTG_pipi_run2", "", cut_pipi, 1);

    // Make list of args to use
    std::map<std::string, RooArgList *> args;
    for (auto mode : modes) {
        args[mode] = new RooArgList();
        args[mode]->add(Bd_M);
        args[mode]->add(KstarK_ID);
        if (mode == "Kpi") args[mode]->add(BDT_Kpi);
        else if (mode == "piK") args[mode]->add(BDT_piK);
        else if (mode == "KK") args[mode]->add(BDT_KK);
        else if (mode == "pipi") args[mode]->add(BDT_pipi);
    }

    // Set up bins
    int binWidth = 5;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // ============
    // Read in data
    // ============
    std::string path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    std::map<std::string, RooDataSet *> data_both;
    std::map<std::string, RooDataSet *> data_plus;
    std::map<std::string, RooDataSet *> data_minus;

    // Loop through modes
    for (auto mode : modes) {

        // Make TChain to hold data
        TChain * chain = new TChain("DecayTree");

        // Loop through years
        for (auto year : years) {
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
    std::string efficiency_path = "/home/pullen/analysis/B02DKstar/Efficiencies/Values/";
    pr.readParams("signal", MC_path + "signal_Kpi.param");
    pr.readParams("Bs", MC_path + "signal_Bs.param");
    pr.readParams("rho", MC_path + "rho_all_PIDcut.param");
    pr.readParams("low", MC_path + "lowMass.param");
    pr.readParams("eff_sel", efficiency_path + "selection_efficiency_average.txt");
    pr.readParams("eff_sel", efficiency_path + "selection_efficiency_average.txt");

    // =================
    // Make signal shape
    // =================
    // Shifts
    RooRealVar * delta_M = new RooRealVar("delta_M", "", 87.26, 80, 90);
    RooRealVar * shift = new RooRealVar("shift", "", 0, -10, 10);

    // Set up parameters
    RooRealVar * signal_mean_fixed = new RooRealVar("signal_mean_fixed", "", pr["signal_mean"]); 
    RooFormulaVar * signal_mean = new RooFormulaVar("signal_mean", "@0 + @1", RooArgList(*signal_mean_fixed, *shift));
    RooRealVar * signal_sigma_L_fixed = new RooRealVar("signal_sigma_L_fixed", "", pr["signal_sigma_L"]);
    RooRealVar * sigma_smear = new RooRealVar("sigma_smear", "", 1, 0.5, 2);
    RooFormulaVar * signal_sigma_L = new RooFormulaVar("signal_sigma_L", "@0 * @1", RooArgList(*signal_sigma_L_fixed, * sigma_smear));
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
    RooRealVar * Bs_sigma_L_fixed = new RooRealVar("Bs_sigma_L_fixed", "", pr["Bs_sigma_L"]);
    RooRealVar * sigma_smear_Bs = new RooRealVar("sigma_smear_Bs", "", 1, 0.5, 2);
    RooFormulaVar * Bs_sigma_L = new RooFormulaVar("Bs_sigma_L", "@0 * @1", RooArgList(*Bs_sigma_L_fixed, *sigma_smear_Bs));
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
    RooRealVar * rho_mean_fixed = new RooRealVar("rho_mean_fixed", "", pr["rho_mean"]);
    RooFormulaVar * rho_mean = new RooFormulaVar("rho_mean", "@0 + @1", RooArgList(*rho_mean_fixed, *shift));
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
    RooRealVar * low_sigma_gamma_010_fixed = new RooRealVar("low_sigma_gamma_010_fixed", "", pr["low_sigma_gamma_010"]);
    RooRealVar * low_sigma_gamma_101_fixed = new RooRealVar("low_sigma_gamma_101_fixed", "", pr["low_sigma_gamma_101"]);
    //RooRealVar * low_sigma_gamma_010_fixed = new RooRealVar("low_sigma_gamma_010_fixed", "", pr["low_sigma_gamma_010"], pr["low_sigma_gamma_010"] - 10, pr["low_sigma_gamma_010"] + 10);
    //RooRealVar * low_sigma_gamma_101_fixed = new RooRealVar("low_sigma_gamma_101_fixed", "", pr["low_sigma_gamma_101"], pr["low_sigma_gamma_101"] - 10, pr["low_sigma_gamma_101"] + 10);
    RooRealVar * low_sigma_pi_010_fixed = new RooRealVar("low_sigma_pi_010_fixed", "", pr["low_sigma_pi_010"], pr["low_sigma_pi_010"] - 10, pr["low_sigma_pi_010"] + 10);
    //RooRealVar * low_sigma_pi_010_fixed = new RooRealVar("low_sigma_pi_010_fixed", "", pr["low_sigma_pi_010"]);
    //RooRealVar * low_sigma_pi_101_fixed = new RooRealVar("low_sigma_pi_101_fixed", "", pr["low_sigma_pi_101"]);
    RooRealVar * low_sigma_pi_101_fixed = new RooRealVar("low_sigma_pi_101_fixed", "", pr["low_sigma_pi_101"], pr["low_sigma_pi_101"] - 10, pr["low_sigma_pi_101"] + 10);

    // Smear widths with factor accounting for differences in MC/data
    RooFormulaVar * low_sigma_gamma_010 = new RooFormulaVar("low_sigma_gamma_010", "@0 * @1", RooArgList(*low_sigma_gamma_010_fixed, *sigma_smear));
    RooFormulaVar * low_sigma_gamma_101 = new RooFormulaVar("low_sigma_gamma_101", "@0 * @1", RooArgList(*low_sigma_gamma_101_fixed, *sigma_smear));
    RooFormulaVar * low_sigma_pi_010 = new RooFormulaVar("low_sigma_pi_010", "@0 * @1", RooArgList(*low_sigma_pi_010_fixed, *sigma_smear));
    RooFormulaVar * low_sigma_pi_101 = new RooFormulaVar("low_sigma_pi_101", "@0 * @1", RooArgList(*low_sigma_pi_101_fixed, *sigma_smear));

    // Csi
    RooRealVar * low_csi_gamma_010 = new RooRealVar("low_csi_gamma_010", "", pr["low_csi_gamma_010"]);
    RooRealVar * low_csi_gamma_101 = new RooRealVar("low_csi_gamma_101", "", pr["low_csi_gamma_101"]);
    //RooRealVar * low_csi_pi_010 = new RooRealVar("low_csi_pi_010", "", pr["low_csi_pi_010"]);
    //RooRealVar * low_csi_pi_101 = new RooRealVar("low_csi_pi_101", "", pr["low_csi_pi_101"]);
    RooRealVar * low_csi_pi_010 = new RooRealVar("low_csi_pi_010", "", pr["low_csi_pi_010"], pr["low_csi_pi_010"] - 0.5, pr["low_csi_pi_010"] + 0.5);
    RooRealVar * low_csi_pi_101 = new RooRealVar("low_csi_pi_101", "", pr["low_csi_pi_101"], pr["low_csi_pi_101"] - 0.5, pr["low_csi_pi_101"] + 0.5);

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
    RooRealVar * Bs_low_sigma_gamma_010_fixed = new RooRealVar("Bs_low_sigma_gamma_010_fixed", "", pr["low_Bs_sigma_gamma_010"]);
    RooRealVar * Bs_low_sigma_gamma_101_fixed = new RooRealVar("Bs_low_sigma_gamma_101_fixed", "", pr["low_Bs_sigma_gamma_101"]);
    //RooRealVar * Bs_low_sigma_pi_010_fixed = new RooRealVar("Bs_low_sigma_pi_010_fixed", "", pr["low_Bs_sigma_pi_010"]);
    //RooRealVar * Bs_low_sigma_pi_101_fixed = new RooRealVar("Bs_low_sigma_pi_101_fixed", "", pr["low_Bs_sigma_pi_101"]);
    RooRealVar * Bs_low_sigma_pi_010_fixed = new RooRealVar("Bs_low_sigma_pi_010_fixed", "", pr["low_Bs_sigma_pi_010"], pr["low_Bs_sigma_pi_010"] - 10, pr["low_Bs_sigma_pi_010"] + 10);
    RooRealVar * Bs_low_sigma_pi_101_fixed = new RooRealVar("Bs_low_sigma_pi_101_fixed", "", pr["low_Bs_sigma_pi_101"], pr["low_Bs_sigma_pi_101"] - 10, pr["low_Bs_sigma_pi_101"] + 15);
    RooFormulaVar * Bs_low_sigma_gamma_010 = new RooFormulaVar("Bs_low_sigma_gamma_010", "@0 * @1", RooArgList(*Bs_low_sigma_gamma_010_fixed, *sigma_smear_Bs));
    RooFormulaVar * Bs_low_sigma_gamma_101 = new RooFormulaVar("Bs_low_sigma_gamma_101", "@0 * @1", RooArgList(*Bs_low_sigma_gamma_101_fixed, *sigma_smear_Bs));
    RooFormulaVar * Bs_low_sigma_pi_010 = new RooFormulaVar("Bs_low_sigma_pi_010", "@0 * @1", RooArgList(*Bs_low_sigma_pi_010_fixed, *sigma_smear_Bs));
    RooFormulaVar * Bs_low_sigma_pi_101 = new RooFormulaVar("Bs_low_sigma_pi_101", "@0 * @1", RooArgList(*Bs_low_sigma_pi_101_fixed, *sigma_smear_Bs));

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
    RooRealVar * low_frac_010_Kpi = new RooRealVar("low_frac_010_Kpi", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_piK = new RooRealVar("low_frac_010_piK", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_piK_plus = new RooRealVar("low_frac_010_piK_plus", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_piK_minus = new RooRealVar("low_frac_010_piK_minus", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_GLW = new RooRealVar("low_frac_010_GLW", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_GLW_plus = new RooRealVar("low_frac_010_GLW_plus", "", 0.5, 0, 1);
    RooRealVar * low_frac_010_GLW_minus = new RooRealVar("low_frac_010_GLW_minus", "", 0.5, 0, 1);
    RooRealVar * Bs_low_frac_010 = new RooRealVar("Bs_low_frac_010", "", 0.5, 0, 1);

    // Combine helicity components for combined fit
    std::map<std::string, RooAddPdf*> low_shapes;
    std::map<std::string, RooAddPdf*> low_shapes_plus;
    std::map<std::string, RooAddPdf*> low_shapes_minus;
    low_shapes["Kpi"] = new RooAddPdf("low_Kpi_shape", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_Kpi));
    low_shapes["piK"] = new RooAddPdf("low_piK_shape", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_piK));
    low_shapes["KK"] = new RooAddPdf("low_KK_shape", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_GLW));
    low_shapes["pipi"] = low_shapes["KK"];
    RooAddPdf * Bs_low_shape = new RooAddPdf("Bs_low_shape", "", 
            RooArgList(*Bs_low_010_shape, *Bs_low_101_shape), RooArgList(*Bs_low_frac_010));

    // Separate shapes for flavour split fit to account for CP violation
    low_shapes_plus["Kpi"] = low_shapes["Kpi"];
    low_shapes_minus["Kpi"] = low_shapes["Kpi"];
    low_shapes_plus["piK"] = new RooAddPdf("low_piK_shape_plus", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_piK_plus));
    low_shapes_minus["piK"] = new RooAddPdf("low_piK_shape_minus", "", 
            RooArgList(*low_010_shape, *low_101_shape), RooArgList(*low_frac_010_piK_minus));
    low_shapes_plus["KK"] = new RooAddPdf("low_KK_shape_plus", "", RooArgList(*low_010_shape, *low_101_shape),
            RooArgList(*low_frac_010_GLW_plus));
    low_shapes_minus["KK"] = new RooAddPdf("low_KK_shape_minus", "", RooArgList(*low_010_shape, *low_101_shape),
            RooArgList(*low_frac_010_GLW_minus));
    low_shapes_plus["pipi"] = low_shapes_plus["KK"];
    low_shapes_minus["pipi"] = low_shapes_minus["KK"];

    // ========================
    // Make combinatorial shape
    // ========================
    std::map<std::string, RooExponential *> expo_shapes;
    RooRealVar * slope_Kpi = new RooRealVar("slope_Kpi", "", -0.005, -0.5, 0.0);
    RooRealVar * slope_KK = new RooRealVar("slope_KK", "", -0.005, -0.5, 0.0);
    RooRealVar * slope_pipi = new RooRealVar("slope_pipi", "", -0.005, -0.5, 0.0);
    expo_shapes["Kpi"] = new RooExponential("expo_Kpi_shape", "", Bd_M, *slope_Kpi);
    expo_shapes["piK"] = expo_shapes["Kpi"];
    expo_shapes["KK"] = new RooExponential("expo_KK_shape", "", Bd_M, *slope_KK);
    expo_shapes["pipi"] = new RooExponential("expo_pipi_shape", "", Bd_M, *slope_pipi);

    // ==================
    // Set up observables
    // ==================
    // Signal asymmetries; a_Kpi is ratio n_minus / n_plus
    RooRealVar * A_Kpi = new RooRealVar("A_Kpi", "", 0, -1, 1);
    RooFormulaVar * a_Kpi = new RooFormulaVar("a_Kpi", "(1 + @0) / (1 - @0)", RooArgList(*A_Kpi));

    // KK and pipi blind asymmetries
    RooRealVar * A_KK_blind = new RooRealVar("A_KK_blind", "", 0, -1, 1);
    RooRealVar * A_pipi_blind = new RooRealVar("A_pipi_blind", "", 0, -1, 1);
    RooUnblindUniform * A_KK = new RooUnblindUniform("A_KK", "", "blind_KK_asym", 0.01, *A_KK_blind);
    RooUnblindUniform * A_pipi = new RooUnblindUniform("A_pipi", "", "blind_pipi_asym", 0.01, *A_pipi_blind);
    RooFormulaVar * a_KK = new RooFormulaVar("a_KK", "(1 + @0) / (1 - @0)", RooArgList(*A_KK));
    RooFormulaVar * a_pipi = new RooFormulaVar("a_pipi", "(1 + @0) / (1 - @0)", RooArgList(*A_pipi));

    // Bs asymmetries (check these for signs etc later)
    RooRealVar * A_piK_Bs = new RooRealVar("A_piK_Bs", "", 0, -1, 1);
    RooRealVar * A_KK_Bs = new RooRealVar("A_KK_Bs", "", 0, -1, 1);
    RooRealVar * A_pipi_Bs = new RooRealVar("A_pipi_Bs", "", 0, -1, 1);
    RooFormulaVar * a_piK_Bs = new RooFormulaVar("a_piK_Bs", "(1 + @0) / (1 - @0)", RooArgList(*A_piK_Bs));
    RooFormulaVar * a_KK_Bs = new RooFormulaVar("a_KK_Bs", "(1 + @0) / (1 - @0)", RooArgList(*A_KK_Bs));
    RooFormulaVar * a_pipi_Bs = new RooFormulaVar("a_pipi_Bs", "(1 + @0) / (1 - @0)", RooArgList(*A_pipi_Bs));

    // Low mass asymmetries
    RooRealVar * A_Kpi_low = new RooRealVar("A_Kpi_low", "", 0, -1, 1);
    RooRealVar * A_KK_low = new RooRealVar("A_KK_low", "", 0, -1, 1);
    RooRealVar * A_pipi_low = new RooRealVar("A_pipi_low", "", 0, -1, 1);
    RooFormulaVar * a_Kpi_low = new RooFormulaVar("a_Kpi_low", "(1 + @0) / (1 - @0)", RooArgList(*A_Kpi_low));
    RooFormulaVar * a_KK_low = new RooFormulaVar("a_KK_low", "(1 + @0) / (1 - @0)", RooArgList(*A_KK_low));
    RooFormulaVar * a_pipi_low = new RooFormulaVar("a_pipi_low", "(1 + @0) / (1 - @0)", RooArgList(*A_pipi_low));

    // Signal ratios
    // Suppressed to favoured ADS ratio
    RooRealVar * R_piK_vs_Kpi_blind = new RooRealVar("R_piK_vs_Kpi_blind", "", 0.06, 0, 1);
    RooUnblindUniform * R_piK_vs_Kpi = new RooUnblindUniform("R_piK_vs_Kpi", "", "blind_piK_ratio", 0.01, *R_piK_vs_Kpi_blind);

    // CP modes to favoured ADS ratio
    RooRealVar * R_KK_vs_Kpi_blind = new RooRealVar("R_KK_vs_Kpi_blind", "", 0.3, 0, 1);
    RooUnblindUniform * R_KK_vs_Kpi = new RooUnblindUniform("R_KK_vs_Kpi", "", "blind_KK_ratio", 0.1, *R_KK_vs_Kpi_blind);
    RooRealVar * R_pipi_vs_Kpi_blind = new RooRealVar("R_pipi_vs_Kpi_blind", "", 0.1, 0, 1);
    RooUnblindUniform * R_pipi_vs_Kpi = new RooUnblindUniform("R_pipi_vs_Kpi", "", "blind_pipi_ratio", 0.05, *R_pipi_vs_Kpi_blind);

    // Flavour split ratios
    RooRealVar * R_plus_blind = new RooRealVar("R_plus_blind", "", 0.06, 0, 1);
    RooRealVar * R_minus_blind = new RooRealVar("R_minus_blind", "", 0.06, 0, 1);
    RooUnblindUniform * R_plus = new RooUnblindUniform("R_plus", "", "blind_R_plus", 0.01, *R_plus_blind);
    RooUnblindUniform * R_minus = new RooUnblindUniform("R_minus", "", "blind_R_minus", 0.01, *R_minus_blind);

    // Bs ratios
    RooRealVar * R_KK_vs_piK_Bs = new RooRealVar("R_KK_vs_piK_Bs", "", 0.3, 0, 1);
    RooRealVar * R_pipi_vs_piK_Bs = new RooRealVar("R_pipi_vs_piK_Bs", "", 0.1, 0, 1);

    // Low mass ratios
    RooRealVar * R_piK_vs_Kpi_low = new RooRealVar("R_piK_vs_Kpi_low", "", 0.06, 0, 1);
    RooRealVar * R_KK_vs_Kpi_low = new RooRealVar("R_KK_vs_Kpi_low", "", 0.3, 0, 1);
    RooRealVar * R_pipi_vs_Kpi_low = new RooRealVar("R_pipi_vs_Kpi_low", "", 0.1, 0, 1);

    // Low mass flavour split ratios
    RooRealVar * R_plus_low = new RooRealVar("R_plus_low", "", 0.06, 0, 1);
    RooRealVar * R_minus_low = new RooRealVar("R_minus_low", "", 0.06, 0, 1);

    // Low mass Bs ratios
    RooRealVar * R_KK_vs_piK_Bs_low = new RooRealVar("R_KK_vs_piK_Bs_low", "", 0.3, 0, 1);
    RooRealVar * R_pipi_vs_piK_Bs_low = new RooRealVar("R_pipi_vs_piK_Bs_low", "", 0.1, 0, 1);

    // =============
    // Set up yields
    // =============
    // Maps to hold yields for each mode
    std::map<std::string, std::map<std::string, RooAbsReal *>> yields;
    std::map<std::string, std::map<std::string, RooAbsReal *>> yields_plus;
    std::map<std::string, std::map<std::string, RooAbsReal *>> yields_minus;

    // Maximum possible yields in each category
    std::map<std::string, int> total_entries;
    for (auto mode : modes) {
        total_entries[mode] = data_both[mode]->sumEntries();
    }

    // Total signal yields
    yields["Kpi"]["n_signal"] = new RooRealVar("n_signal_Kpi", "", 100, 0, total_entries["Kpi"]);
    yields["piK"]["n_signal"] = new RooFormulaVar("n_signal_piK", "@0 * @1", 
            RooArgList(*yields["Kpi"]["n_signal"], *R_piK_vs_Kpi));
    yields["KK"]["n_signal"] = new RooFormulaVar("n_signal_KK", "@0 * @1", RooArgList(*yields["Kpi"]["n_signal"], *R_KK_vs_Kpi));
    yields["pipi"]["n_signal"] = new RooFormulaVar("n_signal_pipi", "@0 * @1", RooArgList(*yields["Kpi"]["n_signal"], *R_pipi_vs_Kpi));

    // Flavour split signal yields
    yields_plus["Kpi"]["n_signal"] = new RooFormulaVar("n_signal_Kpi_plus", "@0 / (1 + @1)", RooArgList(*yields["Kpi"]["n_signal"], *a_Kpi)); 
    yields_minus["Kpi"]["n_signal"] = new RooFormulaVar("n_signal_Kpi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["Kpi"]["n_signal"], *a_Kpi)); 
    yields_plus["KK"]["n_signal"] = new RooFormulaVar("n_signal_KK_plus", "@0 / (1 + @1)", RooArgList(*yields["KK"]["n_signal"], *a_KK)); 
    yields_minus["KK"]["n_signal"] = new RooFormulaVar("n_signal_KK_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["KK"]["n_signal"], *a_KK)); 
    yields_plus["pipi"]["n_signal"] = new RooFormulaVar("n_signal_pipi_plus", "@0 / (1 + @1)", RooArgList(*yields["pipi"]["n_signal"], *a_pipi)); 
    yields_minus["pipi"]["n_signal"] = new RooFormulaVar("n_signal_pipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["pipi"]["n_signal"], *a_pipi)); 

    // Make suppressed mode yields from CP ratios
    yields_plus["piK"]["n_signal"] = new RooFormulaVar("n_signal_piK_plus", "@0 * @1", RooArgList(*yields_plus["Kpi"]["n_signal"], *R_plus));
    yields_minus["piK"]["n_signal"] = new RooFormulaVar("n_signal_piK_minus", "@0 * @1", RooArgList(*yields_minus["Kpi"]["n_signal"], *R_minus));

    // Bs yields
    yields["piK"]["n_Bs"] = new RooRealVar("n_Bs_piK", "", 100, 0, total_entries["piK"]);
    yields["KK"]["n_Bs"] = new RooFormulaVar("n_Bs_KK", "@0 * @1", RooArgList(*yields["piK"]["n_Bs"], *R_KK_vs_piK_Bs));
    yields["pipi"]["n_Bs"] = new RooFormulaVar("n_Bs_pipi", "@0 * @1", RooArgList(*yields["piK"]["n_Bs"], *R_pipi_vs_piK_Bs));

    // Flavour split Bs yields
    yields_plus["piK"]["n_Bs"] = new RooFormulaVar("n_Bs_piK_plus", "@0 / (1 + @1)", RooArgList(*yields["piK"]["n_Bs"], *a_piK_Bs)); 
    yields_minus["piK"]["n_Bs"] = new RooFormulaVar("n_Bs_piK_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["piK"]["n_Bs"], *a_piK_Bs)); 
    yields_plus["KK"]["n_Bs"] = new RooFormulaVar("n_Bs_KK_plus", "@0 / (1 + @1)", RooArgList(*yields["KK"]["n_Bs"], *a_KK_Bs)); 
    yields_minus["KK"]["n_Bs"] = new RooFormulaVar("n_Bs_KK_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["KK"]["n_Bs"], *a_KK_Bs)); 
    yields_plus["pipi"]["n_Bs"] = new RooFormulaVar("n_Bs_pipi_plus", "@0 / (1 + @1)", RooArgList(*yields["pipi"]["n_Bs"], *a_pipi_Bs)); 
    yields_minus["pipi"]["n_Bs"] = new RooFormulaVar("n_Bs_pipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["pipi"]["n_Bs"], *a_pipi_Bs)); 

    // Low mass yields
    yields["Kpi"]["n_low"] = new RooRealVar("n_low_Kpi", "", 100, 0, total_entries["Kpi"]);
    yields["piK"]["n_low"] = new RooFormulaVar("n_low_piK", "@0 * @1", RooArgList(*yields["Kpi"]["n_low"], *R_piK_vs_Kpi_low));
    yields["KK"]["n_low"] = new RooFormulaVar("n_low_KK", "@0 * @1", RooArgList(*yields["Kpi"]["n_low"], *R_KK_vs_Kpi_low));
    yields["pipi"]["n_low"] = new RooFormulaVar("n_low_pipi", "@0 * @1", RooArgList(*yields["Kpi"]["n_low"], *R_pipi_vs_Kpi_low));

    // Flavour split low mass yields
    yields_plus["Kpi"]["n_low"] = new RooFormulaVar("n_low_Kpi_plus", "@0 / (1 + @1)", RooArgList(*yields["Kpi"]["n_low"], *a_Kpi_low));
    yields_minus["Kpi"]["n_low"] = new RooFormulaVar("n_low_Kpi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["Kpi"]["n_low"], *a_Kpi_low));
    yields_plus["KK"]["n_low"] = new RooFormulaVar("n_low_KK_plus", "@0 / (1 + @1)", RooArgList(*yields["KK"]["n_low"], *a_KK_low));
    yields_minus["KK"]["n_low"] = new RooFormulaVar("n_low_KK_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["KK"]["n_low"], *a_KK_low));
    yields_plus["pipi"]["n_low"] = new RooFormulaVar("n_low_pipi_plus", "@0 / (1 + @1)", RooArgList(*yields["pipi"]["n_low"], *a_pipi_low));
    yields_minus["pipi"]["n_low"] = new RooFormulaVar("n_low_pipi_minus", "@0 / (1 + 1/@1)", RooArgList(*yields["pipi"]["n_low"], *a_pipi_low));

    // Split piK low mass yields using ratios
    yields_plus["piK"]["n_low"] = new RooFormulaVar("n_low_piK_plus", "@0 * @1", RooArgList(*yields_plus["Kpi"]["n_low"], *R_plus_low));
    yields_minus["piK"]["n_low"] = new RooFormulaVar("n_low_piK_minus", "@0 * @1", RooArgList(*yields_minus["Kpi"]["n_low"], *R_minus_low));

    // Bs low mass yields
    yields["piK"]["n_Bs_low"] = new RooRealVar("n_Bs_low_piK", "", 100, 0, total_entries["piK"]);
    yields["KK"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_KK", "@0 * @1", RooArgList(*yields["piK"]["n_Bs_low"], *R_KK_vs_piK_Bs_low));
    yields["pipi"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_pipi", "@0 * @1", RooArgList(*yields["piK"]["n_Bs_low"], *R_pipi_vs_piK_Bs_low));

    // Flavour split Bs low mass yields (no asymmetry, shared equally)
    yields_plus["piK"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_piK_plus", "@0 / 2", RooArgList(*yields["piK"]["n_Bs_low"]));
    yields_minus["piK"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_piK_minus", "@0 / 2", RooArgList(*yields["piK"]["n_Bs_low"]));
    yields_plus["KK"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_KK_plus", "@0 / 2", RooArgList(*yields["KK"]["n_Bs_low"]));
    yields_minus["KK"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_KK_minus", "@0 / 2", RooArgList(*yields["KK"]["n_Bs_low"]));
    yields_plus["pipi"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_pipi_plus", "@0 / 2", RooArgList(*yields["pipi"]["n_Bs_low"]));
    yields_minus["pipi"]["n_Bs_low"] = new RooFormulaVar("n_Bs_low_pipi_minus", "@0 / 2", RooArgList(*yields["pipi"]["n_Bs_low"]));

    // Rho yields (shared between piK and Kpi)
    yields["Kpi"]["n_rho"] = new RooRealVar("n_rho_Kpi", "", 50, 0, total_entries["Kpi"]);
    yields["piK"]["n_rho"] = yields["Kpi"]["n_rho"];
    yields["KK"]["n_rho"] = new RooFormulaVar("n_rho_KK", "@0 * @1", RooArgList(*yields["piK"]["n_rho"], *R_KK_vs_piK_Bs));
    yields["pipi"]["n_rho"] = new RooFormulaVar("n_rho_pipi", "@0 * @1", RooArgList(*yields["piK"]["n_rho"], *R_pipi_vs_piK_Bs));

    // Flavour split rho yields (split equally)
    yields_plus["Kpi"]["n_rho"] = new RooFormulaVar("n_rho_Kpi_plus", "@0 / 2", RooArgList(*yields["Kpi"]["n_rho"]));
    yields_minus["Kpi"]["n_rho"] = new RooFormulaVar("n_rho_Kpi_minus", "@0 / 2", RooArgList(*yields["Kpi"]["n_rho"]));
    yields_plus["piK"]["n_rho"] = new RooFormulaVar("n_rho_piK_plus", "@0 / 2", RooArgList(*yields["piK"]["n_rho"]));
    yields_minus["piK"]["n_rho"] = new RooFormulaVar("n_rho_piK_minus", "@0 / 2", RooArgList(*yields["piK"]["n_rho"]));
    yields_plus["KK"]["n_rho"] = new RooFormulaVar("n_rho_KK_plus", "@0 / 2", RooArgList(*yields["KK"]["n_rho"]));
    yields_minus["KK"]["n_rho"] = new RooFormulaVar("n_rho_KK_minus", "@0 / 2", RooArgList(*yields["KK"]["n_rho"]));
    yields_plus["pipi"]["n_rho"] = new RooFormulaVar("n_rho_pipi_plus", "@0 / 2", RooArgList(*yields["pipi"]["n_rho"]));
    yields_minus["pipi"]["n_rho"] = new RooFormulaVar("n_rho_pipi_minus", "@0 / 2", RooArgList(*yields["pipi"]["n_rho"]));

    // Exponential yields
    yields["Kpi"]["n_expo"] = new RooRealVar("n_expo_Kpi", "", total_entries["Kpi"]/2, 0, total_entries["Kpi"]);
    yields["piK"]["n_expo"] = new RooRealVar("n_expo_piK", "", total_entries["piK"], 0, total_entries["piK"]);
    yields["KK"]["n_expo"] = new RooRealVar("n_expo_KK", "", total_entries["KK"], 0, total_entries["KK"]);
    yields["pipi"]["n_expo"] = new RooRealVar("n_expo_pipi", "", total_entries["pipi"], 0, total_entries["pipi"]);

    // Flavour split exponential yields (split equally)
    yields_plus["Kpi"]["n_expo"] = new RooFormulaVar("n_expo_Kpi_plus", "@0 / 2", RooArgList(*yields["Kpi"]["n_expo"]));
    yields_minus["Kpi"]["n_expo"] = new RooFormulaVar("n_expo_Kpi_minus", "@0 / 2", RooArgList(*yields["Kpi"]["n_expo"]));
    yields_plus["piK"]["n_expo"] = new RooFormulaVar("n_expo_piK_plus", "@0 / 2", RooArgList(*yields["piK"]["n_expo"]));
    yields_minus["piK"]["n_expo"] = new RooFormulaVar("n_expo_piK_minus", "@0 / 2", RooArgList(*yields["piK"]["n_expo"]));
    yields_plus["KK"]["n_expo"] = new RooFormulaVar("n_expo_KK_plus", "@0 / 2", RooArgList(*yields["KK"]["n_expo"]));
    yields_minus["KK"]["n_expo"] = new RooFormulaVar("n_expo_KK_minus", "@0 / 2", RooArgList(*yields["KK"]["n_expo"]));
    yields_plus["pipi"]["n_expo"] = new RooFormulaVar("n_expo_pipi_plus", "@0 / 2", RooArgList(*yields["pipi"]["n_expo"]));
    yields_minus["pipi"]["n_expo"] = new RooFormulaVar("n_expo_pipi_minus", "@0 / 2", RooArgList(*yields["pipi"]["n_expo"]));

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
            shapes_list.add(*(rho_shape));
            if (mode != "Kpi") {
                shapes_list.add(*Bs_shape);
                shapes_list.add(*Bs_low_shape);
            }

            // Add yields
            yields_list.add(*(yields[mode]["n_signal"]));
            yields_list.add(*(yields[mode]["n_expo"]));
            yields_list.add(*(yields[mode]["n_low"]));
            yields_list.add(*(yields[mode]["n_rho"]));
            if (mode != "Kpi") {
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
            if (mode != "Kpi") {
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
            if (mode != "Kpi") {
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
    RooFitResult * data_result;
    if (binned == "Y") {
        data_result = simPdf->fitTo(*((RooDataHist*)combData), RooFit::Save(), RooFit::NumCPU(8, 2),
                RooFit::Optimize(false), RooFit::Offset(true), 
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    } else {
        data_result = simPdf->fitTo(*((RooDataSet*)combData), RooFit::Save(), RooFit::NumCPU(8, 2),
                RooFit::Optimize(false), RooFit::Offset(true), 
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    }
    data_result->Print("v");

    // Make name for results output
    std::string sum_string = ((sum == "Y") ? "combined" : "split");
    std::string bin_string = ((binned == "Y") ? "binned" : "unbinned");
    std::string result_filename;
    std::stringstream bdt_stream;

    // If doing the proper data fit, save RooFitResult
    if (toys != "Y") {
        if (custom_cuts) {
            bdt_stream << "_" << std::setprecision(2) << cut_Kpi << "_" << cut_piK << "_"
                << cut_KK << "_" << cut_pipi;
            result_filename = "../Results/BDT_studies/twoBody_" + input_year + "_" +
                sum_string + "_" + bin_string + bdt_stream.str() + ".root";
        } else {
            result_filename = "../Results/twoBody_" + input_year + "_" + sum_string + 
                "_" + bin_string + ".root";
        }
        std::cout << "Will save result to: " << result_filename << std::endl;

        // Save result to file
        TFile * result_file = TFile::Open(result_filename.c_str(), "RECREATE");
        result_file->cd();
        data_result->Write("fit_result");
        result_file->Close();
    }

    // ================================
    // Save results to a histogram file (only if not doing BDT study)
    // ================================
    if (save_plots == "Y") {

        // Open file
        std::string outfile_name;
        if (custom_cuts) {
            outfile_name = "../Histograms/BDT_studies/fits_twoBody_" + sum_string + "_" + bin_string + bdt_stream.str() + ".root";
        } else {
            outfile_name = "../Histograms/fits_twoBody_" + sum_string + "_" + bin_string + ".root";
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
                if (mode == "Kpi") {

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
                        if (bin_val > lsb && bin_val < usb) h_data_blind->SetBinError(bin, 0);
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
        if (custom_cuts) {

            // Save to a subdirectory for BDT study plots
            if (sum == "Y") {
                plotter->plotFourModeFitsCombined(("../Histograms/BDT_studies/fits_twoBody_combined_" + bin_string + bdt_stream.str() + ".root").c_str(), 
                        "BDT_studies/twoBody_" + input_year + "_" + bin_string + "_" + bdt_stream.str(), "");
            } else {
                plotter->plotFourModeFitsSeparate(("../Histograms/BDT_studies/fits_twoBody_split_" + bin_string + bdt_stream.str() + ".root").c_str(), 
                        "BDT_studies/twoBody_" + input_year + "_" + bin_string + "_" + bdt_stream.str(), "");
            }
        } else {

            // If using standard cuts, save to the usual plot directory
            if (sum == "Y") {
                plotter->plotFourModeFitsCombined(("../Histograms/fits_twoBody_combined_" + bin_string + ".root").c_str(), 
                        "twoBody_" + input_year + "_" + bin_string, "");
            } else {
                plotter->plotFourModeFitsSeparate(("../Histograms/fits_twoBody_split_" + bin_string + ".root").c_str(), 
                        "twoBody_" + input_year + "_" + bin_string, "");
            }
        }
        delete plotter;
    }

    // Run toy fits if requested
    if (toys == "Y") {

        std::cout << "Performing toy study" << std::endl;

        // Generate random number to put into toy file
        RooRandom::randomGenerator()->SetSeed(0);
        TRandom r3(0);
        double rand = r3.Uniform();
        std::string rand_str = std::to_string(rand);

        // Number of toys to generate
        int n_toys = 2;

        // Number of events to generate
        int n_events_per_sample = simPdf->expectedEvents(category);

        // RooMCStudy to handle toy generation
        RooMCStudy * mc_study_gen = new RooMCStudy(*simPdf, RooArgList(Bd_M, category),
                RooFit::Binned(true), RooFit::Silence(false), RooFit::Extended(true),
                RooFit::FitOptions(RooFit::NumCPU(8,2), RooFit::Extended(true),
                    RooFit::Optimize(false), RooFit::Offset(true),
                    RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2)));
        RooFitResult * toys_result[1000] = {0};
        RooDataSet * toy_data = 0;
        RooSimultaneous * simPdf_toys = 0;

        // Generate toys
        mc_study_gen->generate(n_toys, n_events_per_sample, true, 0);

        // Loop over generated datasets and run fits for each
        for (int i = 0; i < n_toys; i++) {

            // Get dataset
            toy_data = (RooDataSet*)mc_study_gen->genData(i);
            
            // Clone initial PDF
            simPdf_toys = (RooSimultaneous*)simPdf->Clone();

            // Fit generated dataset with the initial PDF
            toys_result[i] = simPdf_toys->fitTo(*toy_data, RooFit::Save(), 
                    RooFit::NumCPU(8, 2), RooFit::Extended(true), 
                    RooFit::Optimize(false), RooFit::Offset(true), 
                    RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
        } // End loop over toy fitting

        // Output file for results
        TFile * output = TFile::Open(("/data/lhcb/users/pullen/B02DKstar/"
                    "toys/twoBody/toy_results_nToys_" + rand_str + "_nToys_" 
                    + std::to_string(n_toys) + ".root").c_str(), "RECREATE");

        // Tree to store initial and final parameters, and fit status
        TTree * toy_tree = new TTree("toy_tree", "");
        TIterator * it_init;
        TIterator * it_final;
        std::string study_var_init_name = "";
        std::string study_var_final_name = "";
        RooRealVar * study_var_init;
        RooRealVar * study_var_final;

        // Get list of floating variables from the MCStudy
        RooArgList float_vars = toys_result[0]->floatParsFinal();

        // Iterate over fit variables
        TIterator * it = float_vars.createIterator();

        // Number of non-convergent fits
        int n_bad = 0;

        // Number forced pos-def
        int n_posdef = 0;

        // Make branches for bad and forced pos-def fits
        toy_tree->Branch("n_bad", & n_bad, "n_bad/I");
        toy_tree->Branch("n_forced_posdef", & n_posdef, "n_forced_posdef/I");

        // Branches for boolean bad/forced pos-def
        bool is_bad = false;
        bool is_posdef = false;
        toy_tree->Branch("is_bad", &is_bad, "is_bad/B");
        toy_tree->Branch("is_forced_posdef", &is_posdef, "is_forced_posdef/B");

        // EDM of toy fit
        bool low_EDM = false;
        double EDM = 0;
        toy_tree->Branch("low_EDM", &low_EDM, "low_EDM/B");
        toy_tree->Branch("EDM", &EDM, "EDM/D");

        // Status and covariance quality
        double status = 0;
        double covqual = 0;
        toy_tree->Branch("status", &status, "status/D");
        toy_tree->Branch("covQual", &covqual, "covQual/D");

        // RooRealVars to use in correlation plotting
        std::map<int, RooRealVar *> varMap;
        std::map<std::string, double> init_values;
        std::map<std::string, double> final_values;
        std::map<std::string, double> error_values;

        // Loop through variables
        std::string var_name = "";
        RooRealVar * var;
        int n_vars = 0;
        while ((var = (RooRealVar*)it->Next())) {

            // Get variable name
            var_name = var->GetName();
            n_vars++;

            // Add branches for the variable
            toy_tree->Branch(("init_value_" + var_name).c_str(), 
                    &init_values[var_name], ("init_value_" + var_name + "/D").c_str());
            toy_tree->Branch(("final_value_" + var_name).c_str(), 
                    &final_values[var_name], ("final_value_" + var_name + "/D").c_str());
            toy_tree->Branch(("error_value_" + var_name).c_str(), 
                    &error_values[var_name], ("error_value_" + var_name + "/D").c_str());

        } // End loop over floating variable names
        std::cout << "Pulls will be plotted for " << n_vars << " variables."
            << std::endl;

        // Loop over different fit results
        std::cout << "Looping over toy fit results to calculate pulls" << std::endl;
        for (int i = 0; i < n_toys; i++) {

            // Save RooFitResult to file
            toys_result[i]->Write(("Toy_" + std::to_string(i)).c_str());

            // Check for failed fits
            if (toys_result[i]->covQual() < 2) {
                n_bad++;
                is_bad = true;
            } else {
                is_bad = false;
            }
            if (toys_result[i]->covQual() <3) {
                n_posdef++;
                is_posdef = kTRUE;
            } else {
                is_posdef = false;
            }
            EDM = toys_result[i]->edm();
            covqual = toys_result[i]->covQual();
            status = toys_result[i]->status();
            low_EDM = (EDM < 1.0);

            // Get initial values
            it_init = (TIterator*)data_result->floatParsFinal().createIterator();
            while ((study_var_init = (RooRealVar*)it_init->Next())) {

                // Get variable name
                study_var_init_name = study_var_init->GetName();

                // Fill values of initial variables
                for (auto init_var : init_values) {
                    if (init_var.first == study_var_init_name) {
                        init_values[study_var_init_name] = study_var_init->getVal();
                    }
                }
            } // End loop over toy initial parameters

            // Get final values and their errors
            it_final = (TIterator*)toys_result[i]->floatParsFinal().createIterator();
            while ((study_var_final = (RooRealVar*)it_final->Next())) {

                // Get variable name
                study_var_final_name = study_var_final->GetName();

                // Fill values of final variables
                for (auto final_var : final_values) {
                    if (final_var.first == study_var_final_name) {
                        final_values[study_var_final_name] = study_var_final->getVal();
                        error_values[study_var_final_name] = study_var_final->getError();
                    }
                }
            } // End loop over toy final parameters

            // Fill the tree
            toy_tree->Fill();
            is_bad = false;
            is_posdef = false;
        } // End loop over toy fit results

        // Save 
        toy_tree->AutoSave();
        output->Write();
        output->Close();
    }

    return 0;
}
