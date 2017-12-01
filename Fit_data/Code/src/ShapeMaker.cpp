#include "TH1F.h"
#include "TFile.h"

#include "RooAddPdf.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooHist.h"
#include "RooArgList.h"
#include "RooDataSet.h"
#include "RooCategory.h"
#include "RooCBShape.h"
#include "RooFormulaVar.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooExponential.h"
#include "RooUnblindUniform.h"

#include "RooHILLdini.h"
#include "RooHORNSdini.h"
#include "RooLITTLEHORNSdini.h"

#include "ParameterReader.hpp"
#include "ShapeMaker.hpp"

// ===========
// Constructor
// ===========
ShapeMaker::ShapeMaker(std::string sum, RooRealVar * Bd_M) : 
    m_sum(sum), m_x(Bd_M) {

    // List of modes
    m_modes = {"Kpi", "piK", "KK", "pipi"};

    // Set up category    
    m_cat = new RooCategory("category", "");
    for (auto mode : m_modes) {
        if (m_sum == "Y") {
            m_cat->defineType(mode.c_str());
        } else {
            m_cat->defineType((mode + "_plus").c_str());
            m_cat->defineType((mode + "_minus").c_str());
        }
    }

    // Read in MC fit parameters
    std::string MC_path = 
        "/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Results/";
    m_pr = new ParameterReader();
    m_pr->readParams("signal", MC_path + "signal_Kpi.param");
    m_pr->readParams("Bs", MC_path + "signal_Bs.param");
    m_pr->readParams("rho", MC_path + "rho_all_PIDcut.param");
    m_pr->readParams("low", MC_path + "lowMass.param");

}


// ==========
// Destructor
// ==========
ShapeMaker::~ShapeMaker() {
    delete m_pr;
}


// ============================
// Make a PDF to use in fitting
// ============================
RooSimultaneous * ShapeMaker::makeFitPdf() {

    // ====================
    // Set up floating vars
    // ====================
    // Shifts
    m_fit_vars["delta_M"] = new RooRealVar("delta_M", "", 87.26, 80, 90);
    m_fit_vars["shift"] = new RooRealVar("shift", "", 0, -10, 10);

    // Signal parameters
    m_fit_vars["signal_mean_free"] = new RooRealVar("signal_mean_free", "", 
            m_pr->getParam("signal_mean")); 
    m_fit_vars["signal_mean"] = new RooFormulaVar("signal_mean", "@0 + @1", 
                RooArgList(*m_fit_vars.at("signal_mean_free"), 
                           *m_fit_vars.at("shift")));
    m_fit_vars["signal_sigma_L"] = new RooRealVar("signal_sigma_L", "", 
            m_pr->getParam("signal_sigma_L"), 
            m_pr->getParam("signal_sigma_L") - 10, 
            m_pr->getParam("signal_sigma_L") + 10);

    // Bs floating parameters
    m_fit_vars["Bs_mean"] = new RooFormulaVar("Bs_mean", "@0 + @1", 
            RooArgList(*m_fit_vars.at("signal_mean"), 
                       *m_fit_vars.at("delta_M")));
    m_fit_vars["Bs_sigma_L"] = new RooRealVar("Bs_sigma_L", "", 
            m_pr->getParam("Bs_sigma_L"), 
            m_pr->getParam("Bs_sigma_L") - 10, 
            m_pr->getParam("Bs_sigma_L") + 10);

    // Floating fraction of 010 shape
    m_fit_vars["low_frac_010_Kpi"] = new RooRealVar("low_frac_010_Kpi", "", 
            0.5, 0, 1);
    m_fit_vars["low_frac_010_piK"] = new RooRealVar("low_frac_010_piK", "", 
            0.5, 0, 1);
    m_fit_vars["low_frac_010_piK_plus"] = new RooRealVar("low_frac_010_piK_plus", 
            "", 0.5, 0, 1);
    m_fit_vars["low_frac_010_piK_minus"] = new RooRealVar("low_frac_010_piK_minus", 
            "", 0.5, 0, 1);
    m_fit_vars["low_frac_010_GLW"] = new RooRealVar("low_frac_010_GLW", "", 
            0.5, 0, 1);
    m_fit_vars["low_frac_010_GLW_plus"] = new RooRealVar("low_frac_010_GLW_plus", 
            "", 0.5, 0, 1);
    m_fit_vars["low_frac_010_GLW_minus"] = new RooRealVar(
            "low_frac_010_GLW_minus", "", 0.5, 0, 1);
    m_fit_vars["Bs_low_frac_010"] = new RooRealVar("Bs_low_frac_010", "", 
            0.5, 0, 1);

    // Slope of exponentials
    m_fit_vars["slope_Kpi"] = new RooRealVar("slope_Kpi", "", -0.005, -0.5, 0.0);
    m_fit_vars["slope_KK"] = new RooRealVar("slope_KK", "", -0.005, -0.5, 0.0);
    m_fit_vars["slope_pipi"] = new RooRealVar("slope_pipi", "", -0.005, -0.5, 0.0);

    // ===========================
    // Set up floating observables
    // ===========================
    // Signal asymmetries
    m_fit_vars["A_Kpi"] = new RooRealVar("A_Kpi", "", 0, -1, 1);

    // KK and pipi blind asymmetries
    m_fit_vars["A_KK_blind"] = new RooRealVar("A_KK_blind", "", 0, -1, 1);
    m_fit_vars["A_pipi_blind"] = new RooRealVar("A_pipi_blind", "", 0, -1, 1);
    m_fit_vars["A_KK"] = new RooUnblindUniform("A_KK", "", "blind_KK_asym", 0.01, 
            *m_fit_vars.at("A_KK_blind"));
    m_fit_vars["A_pipi"] = new RooUnblindUniform("A_pipi", "", 
            "blind_pipi_asym", 0.01, *m_fit_vars.at("A_pipi_blind"));

    // Bs asymmetries (check these for signs etc later)
    m_fit_vars["A_piK_Bs"] = new RooRealVar("A_piK_Bs", "", 0, -1, 1);
    m_fit_vars["A_KK_Bs"] = new RooRealVar("A_KK_Bs", "", 0, -1, 1);
    m_fit_vars["A_pipi_Bs"] = new RooRealVar("A_pipi_Bs", "", 0, -1, 1);

    // Low mass asymmetries
    m_fit_vars["A_Kpi_low"] = new RooRealVar("A_Kpi_low", "", 0, -1, 1);
    m_fit_vars["A_KK_low"] = new RooRealVar("A_KK_low", "", 0, -1, 1);
    m_fit_vars["A_pipi_low"] = new RooRealVar("A_pipi_low", "", 0, -1, 1);

    // Signal ratios
    // Suppressed to favoured ADS ratio
    m_fit_vars["R_piK_vs_Kpi_blind"] = new RooRealVar("R_piK_vs_Kpi_blind", "", 
            0.06, 0, 1);
    m_fit_vars["R_piK_vs_Kpi"] = new RooUnblindUniform("R_piK_vs_Kpi", "", 
            "blind_piK_ratio", 0.01, *m_fit_vars.at("R_piK_vs_Kpi_blind"));

    // CP modes to favoured ADS ratio
    m_fit_vars["R_KK_vs_Kpi_blind"] = new RooRealVar("R_KK_vs_Kpi_blind", "", 
            0.3, 0, 1);
    m_fit_vars["R_KK_vs_Kpi"] = new RooUnblindUniform("R_KK_vs_Kpi", "", 
            "blind_KK_ratio", 0.1, *m_fit_vars.at("R_KK_vs_Kpi_blind"));
    m_fit_vars["R_pipi_vs_Kpi_blind"] = new RooRealVar("R_pipi_vs_Kpi_blind", "", 
            0.1, 0, 1);
    m_fit_vars["R_pipi_vs_Kpi"] = new RooUnblindUniform("R_pipi_vs_Kpi", "", 
            "blind_pipi_ratio", 0.05, *m_fit_vars.at("R_pipi_vs_Kpi_blind"));

    // Flavour split ratios
    m_fit_vars["R_plus_blind"] = new RooRealVar("R_plus_blind", "", 0.06, 0, 1);
    m_fit_vars["R_minus_blind"] = new RooRealVar("R_minus_blind", "", 0.06, 0, 1);
    m_fit_vars["R_plus"] = new RooUnblindUniform("R_plus", "", "blind_R_plus", 
            0.01, *m_fit_vars.at("R_plus_blind"));
    m_fit_vars["R_minus"] = new RooUnblindUniform("R_minus", "", "blind_R_minus", 
            0.01, *m_fit_vars.at("R_minus_blind"));

    // Bs ratios
    m_fit_vars["R_KK_vs_piK_Bs"] = new RooRealVar("R_KK_vs_piK_Bs", "", 0.3, 0, 1);
    m_fit_vars["R_pipi_vs_piK_Bs"] = new RooRealVar("R_pipi_vs_piK_Bs", "", 
            0.1, 0, 1);

    // Low mass ratios
    m_fit_vars["R_piK_vs_Kpi_low"] = new RooRealVar("R_piK_vs_Kpi_low", "", 
            0.06, 0, 1);
    m_fit_vars["R_KK_vs_Kpi_low"] = new RooRealVar("R_KK_vs_Kpi_low", "", 
            0.3, 0, 1);
    m_fit_vars["R_pipi_vs_Kpi_low"] = new RooRealVar("R_pipi_vs_Kpi_low", "", 
            0.1, 0, 1);

    // Low mass flavour split ratios
    m_fit_vars["R_plus_low"] = new RooRealVar("R_plus_low", "", 0.06, 0, 1);
    m_fit_vars["R_minus_low"] = new RooRealVar("R_minus_low", "", 0.06, 0, 1);

    // Low mass Bs ratios
    m_fit_vars["R_KK_vs_piK_Bs_low"] = new RooRealVar("R_KK_vs_piK_Bs_low", "", 
            0.3, 0, 1);
    m_fit_vars["R_pipi_vs_piK_Bs_low"] = new RooRealVar("R_pipi_vs_piK_Bs_low", "", 
            0.1, 0, 1);

    // ====================
    // Make floating yields
    // ====================
    m_fit_vars["n_signal_Kpi"] = new RooRealVar("n_signal_Kpi", "", 100, 0, 7000);
    m_fit_vars["n_Bs_piK"] = new RooRealVar("n_Bs_piK", "", 100, 0, 12000);
    m_fit_vars["n_low_Kpi"] = new RooRealVar("n_low_Kpi", "", 100, 0, 7000);
    m_fit_vars["n_Bs_low_piK"] = new RooRealVar("n_Bs_low_piK", "", 100, 0, 12000);
    m_fit_vars["n_rho_Kpi"] = new RooRealVar("n_rho_Kpi", "", 50, 0, 7000);
    m_fit_vars["n_expo_Kpi"] = new RooRealVar("n_expo_Kpi", "", 7000/2, 0, 7000);
    m_fit_vars["n_expo_piK"] = new RooRealVar("n_expo_piK", "", 12000/2, 0, 12000);
    m_fit_vars["n_expo_KK"] = new RooRealVar("n_expo_KK", "", 2500/2, 0, 2500);
    m_fit_vars["n_expo_pipi"] = new RooRealVar("n_expo_pipi", "", 1000/2, 0, 1000);

    // Return simultaneous PDF
    return makePdf(m_fit_vars, m_fit_pdfs);
}


// ==================================================
// Make a PDF to generate toys using data fit results
// ==================================================
RooSimultaneous * ShapeMaker::makeGenerationPdf(std::string results_file) {

    // Read in RooFitResult
    std::map<std::string, double> * results = readFitResult(results_file);

    // Set up floating vars

    //m_gen_vars[m_gen_var 

    // Call makePdf function

    // Return simultaneous PDF
    return 0;
}


// ==========================================================
// Return the RooCategory used in making the simultaneous PDF
// ==========================================================
RooCategory * ShapeMaker::getCategory() {
    return m_cat;
}

// ===============================================
// Make a PDF after floating vars have been set up
// ===============================================
RooSimultaneous * ShapeMaker::makePdf(VarMap & vars, PdfMap & pdfs) {

    // =======================
    // Set up fixed parameters
    // =======================
    // Signal parameters
    vars["signal_sigma_ratio"] = new RooRealVar("signal_sigma_ratio", "", 
            m_pr->getParam("signal_sigma_ratio"));
    vars["signal_sigma_R"] = new RooFormulaVar("signal_sigma_R", "@0 * @1", 
            RooArgList(*vars.at("signal_sigma_L"), 
                       *vars.at("signal_sigma_ratio")));
    vars["signal_alpha_L"] = new RooRealVar("signal_alpha_L", "", 
            m_pr->getParam("signal_alpha_L"));
    vars["signal_alpha_R"] = new RooRealVar("signal_alpha_R", "", 
            m_pr->getParam("signal_alpha_R"));
    vars["signal_n_L"] = new RooRealVar("signal_n_L", "", m_pr->getParam("signal_n_L"));
    vars["signal_n_R"] = new RooRealVar("signal_n_R", "", m_pr->getParam("signal_n_R"));
    vars["signal_frac"] = new RooRealVar("signal_frac", "", 
            m_pr->getParam("signal_frac"));

    // Bs parameters
    vars["Bs_sigma_ratio"] = new RooRealVar("Bs_sigma_ratio", "", 
            (*m_pr)["Bs_sigma_ratio"]);
    vars["Bs_sigma_R"] = new RooFormulaVar("Bs_sigma_R", "@0 * @1", 
            RooArgList(*vars.at("Bs_sigma_L"), 
                       *vars.at("Bs_sigma_ratio")));
    vars["Bs_alpha_L"] = new RooRealVar("Bs_alpha_L", "", m_pr->getParam("Bs_alpha_L"));
    vars["Bs_alpha_R"] = new RooRealVar("Bs_alpha_R", "", m_pr->getParam("Bs_alpha_R"));
    vars["Bs_n_L"] = new RooRealVar("Bs_n_L", "", m_pr->getParam("Bs_n_L"));
    vars["Bs_n_R"] = new RooRealVar("Bs_n_R", "", m_pr->getParam("Bs_n_R"));
    vars["Bs_frac"] = new RooRealVar("Bs_frac", "", m_pr->getParam("Bs_frac"));

    // Rho mis-ID shape
    vars["rho_mean_fixed"] = new RooRealVar("rho_mean_fixed", "", 
            m_pr->getParam("rho_mean"));
    vars["rho_mean"] = new RooFormulaVar("rho_mean", "@0 + @1", 
            RooArgList(*vars.at("rho_mean_fixed"), *vars.at("shift")));
    vars["rho_sigma_L"] = new RooRealVar("rho_sigma_L", "", 
            m_pr->getParam("rho_sigma_L"));
    vars["rho_sigma_ratio"] = new RooRealVar("rho_sigma_ratio", "", 
            m_pr->getParam("rho_sigma_ratio"));
    vars["rho_sigma_R"] = new RooFormulaVar("rho_sigma_R", "@0 * @1", 
            RooArgList(*vars.at("rho_sigma_L"), *vars.at("rho_sigma_ratio")));
    vars["rho_alpha_L"] = new RooRealVar("rho_alpha_L", "", 
            m_pr->getParam("rho_alpha_L"));
    vars["rho_alpha_R"] = new RooRealVar("rho_alpha_R", "", 
            m_pr->getParam("rho_alpha_R"));
    vars["rho_n_L"] = new RooRealVar("rho_n_L", "", m_pr->getParam("rho_n_L"));
    vars["rho_n_R"] = new RooRealVar("rho_n_R", "", m_pr->getParam("rho_n_R"));
    vars["rho_frac"] = new RooRealVar("rho_frac", "", m_pr->getParam("rho_frac"));

    // Low mass B0 parameters
    vars["low_a_gamma"] = new RooRealVar("low_a_gamma", "", 
            m_pr->getParam("low_a_gamma"));
    vars["low_b_gamma"] = new RooRealVar("low_b_gamma", "", 
            m_pr->getParam("low_b_gamma"));
    vars["low_a_pi"] = new RooRealVar("low_a_pi", "", m_pr->getParam("low_a_pi"));
    vars["low_b_pi"] = new RooRealVar("low_b_pi", "", m_pr->getParam("low_b_pi"));
    vars["low_sigma_gamma_010"] = new RooRealVar("low_sigma_gamma_010", "", 
            m_pr->getParam("low_sigma_gamma_010"));
    vars["low_sigma_gamma_101"] = new RooRealVar("low_sigma_gamma_101", "", 
            m_pr->getParam("low_sigma_gamma_101"));
    vars["low_sigma_pi_010"] = new RooRealVar("low_sigma_pi_010", "", 
            m_pr->getParam("low_sigma_pi_010"));
    vars["low_sigma_pi_101"] = new RooRealVar("low_sigma_pi_101", "", 
            m_pr->getParam("low_sigma_pi_101"));
    vars["low_csi_gamma_010"] = new RooRealVar("low_csi_gamma_010", "", 
            m_pr->getParam("low_csi_gamma_010"));
    vars["low_csi_gamma_101"] = new RooRealVar("low_csi_gamma_101", "", 
            m_pr->getParam("low_csi_gamma_101"));
    vars["low_csi_pi_010"] = new RooRealVar("low_csi_pi_010", "", 
            m_pr->getParam("low_csi_pi_010"));
    vars["low_csi_pi_101"] = new RooRealVar("low_csi_pi_101", "", 
            m_pr->getParam("low_csi_pi_101"));
    vars["low_frac"] = new RooRealVar("low_frac", "", m_pr->getParam("low_frac"));
    vars["low_ratio"] = new RooRealVar("low_ratio", "", 
            m_pr->getParam("low_ratio"));
    vars["low_shiftg"] = new RooRealVar("low_shiftg", "", 0);

    // Low mass Bs parameters
    vars["Bs_low_a_gamma"] = new RooFormulaVar("Bs_low_a_gamma", "@0 + @1", 
            RooArgList(*vars.at("low_a_gamma"), *vars.at("delta_M")));
    vars["Bs_low_b_gamma"] = new RooFormulaVar("Bs_low_b_gamma", "@0 + @1", 
            RooArgList(*vars.at("low_b_gamma"), *vars.at("delta_M")));
    vars["Bs_low_a_pi"] = new RooFormulaVar("Bs_low_a_pi", "@0 + @1", 
            RooArgList(*vars.at("low_a_pi"), *vars.at("delta_M")));
    vars["Bs_low_b_pi"] = new RooFormulaVar("Bs_low_b_pi", "@0 + @1", 
            RooArgList(*vars.at("low_b_pi"), *vars.at("delta_M")));
    vars["Bs_low_sigma_gamma_010"] = new RooRealVar("Bs_low_sigma_gamma_010", 
            "", m_pr->getParam("low_Bs_sigma_gamma_010"));
    vars["Bs_low_sigma_gamma_101"] = new RooRealVar(
            "Bs_low_sigma_gamma_101", "", m_pr->getParam("low_Bs_sigma_gamma_101"));
    vars["Bs_low_sigma_pi_010"] = new RooRealVar("Bs_low_sigma_pi_010", "", 
            m_pr->getParam("low_Bs_sigma_pi_010"));
    vars["Bs_low_sigma_pi_101"] = new RooRealVar("Bs_low_sigma_pi_101", "", 
            m_pr->getParam("low_Bs_sigma_pi_101"));
    vars["Bs_low_csi_gamma_010"] = new RooRealVar("Bs_low_csi_gamma_010", 
            "", m_pr->getParam("low_Bs_csi_gamma_010"));
    vars["Bs_low_csi_gamma_101"] = new RooRealVar("Bs_low_csi_gamma_101", 
            "", m_pr->getParam("low_Bs_csi_gamma_101"));
    vars["Bs_low_csi_pi_010"] = new RooRealVar("Bs_low_csi_pi_010", "", 
            m_pr->getParam("low_Bs_csi_pi_010"));
    vars["Bs_low_csi_pi_101"] = new RooRealVar("Bs_low_csi_pi_101", "", 
            m_pr->getParam("low_Bs_csi_pi_101"));

    // Paramaters for combining low mass shapes
    const double branching_gamma = 0.353;
    const double branching_pi = 0.647;
    const double acc_gamma = 17.48;
    const double acc_pi = 15.53;
    double G_010 = branching_gamma * acc_gamma * 0.009227;
    double G_101 = branching_gamma * acc_gamma * 0.004613; 
    double P_010 = branching_pi * acc_pi * 0.01168;
    double P_101 = branching_pi * acc_pi * 0.005712;
    vars["coeff_gamma_010"] = new RooRealVar("coeff_gamma_010", "", 
            G_010 / (P_010 + G_010));
    vars["coeff_gamma_101"] = new RooRealVar("coeff_gamma_101", "", 
            G_101 / (P_101 + G_101));
    vars["coeff_pi_010"] = new RooRealVar("coeff_pi_010", "", 
            P_010 / (P_010 + G_010));
    vars["coeff_pi_101"] = new RooRealVar("coeff_pi_101", "", 
            P_101 / (P_101 + G_101));

    // =========
    // Make PDFs
    // =========
    // Signal shape
    pdfs["signal_CB_L"] = new RooCBShape("signal_CB_L", "", *m_x, 
            *vars.at("signal_mean"), 
            *vars.at("signal_sigma_L"), 
            *vars.at("signal_alpha_L"), 
            *vars.at("signal_n_L"));
    pdfs["signal_CB_R"] = new RooCBShape("signal_CB_R", "", *m_x, 
            *vars.at("signal_mean"), 
            *vars.at("signal_sigma_R"), 
            *vars.at("signal_alpha_R"), 
            *vars.at("signal_n_R"));
    pdfs["signal_shape"] = new RooAddPdf("signal_shape", "", 
            RooArgList(*pdfs.at("signal_CB_L"), *pdfs.at("signal_CB_R")), 
            RooArgList(*vars.at("signal_frac")));

    // Bs shape
    pdfs["Bs_CB_L"] = new RooCBShape("Bs_CB_L", "", *m_x, 
            *vars.at("Bs_mean"), 
            *vars.at("Bs_sigma_L"), 
            *vars.at("Bs_alpha_L"), 
            *vars.at("Bs_n_L"));
    pdfs["Bs_CB_R"] = new RooCBShape("Bs_CB_R", "", *m_x, 
            *vars.at("Bs_mean"), 
            *vars.at("Bs_sigma_R"), 
            *vars.at("Bs_alpha_R"), 
            *vars.at("Bs_n_R"));
    pdfs["Bs_shape"] = new RooAddPdf("Bs_shape", "", 
            RooArgList(*pdfs.at("Bs_CB_L"), *pdfs.at("Bs_CB_R")), 
            RooArgList(*vars.at("Bs_frac")));

    // Rho mis-ID shape
    pdfs["rho_CB_L"] = new RooCBShape("rho_CB_L", "", *m_x, 
            *vars.at("rho_mean"), 
            *vars.at("rho_sigma_L"), 
            *vars.at("rho_alpha_L"), 
            *vars.at("rho_n_L"));
    pdfs["rho_CB_R"] = new RooCBShape("rho_CB_R", "", *m_x, 
            *vars.at("rho_mean"), 
            *vars.at("rho_sigma_R"), 
            *vars.at("rho_alpha_R"), 
            *vars.at("rho_n_R"));
    pdfs["rho_shape"] = new RooAddPdf("rho_shape", "", 
            RooArgList(*pdfs.at("rho_CB_L"), *pdfs.at("rho_CB_R")), 
            RooArgList(*vars.at("rho_frac")));

    // Low mass B0 shapes
    pdfs["gamma_010_shape"] = new RooHILLdini("gamma_010_shape", "", *m_x, 
            *vars.at("low_a_gamma"), 
            *vars.at("low_b_gamma"), 
            *vars.at("low_csi_gamma_010"), 
            *vars.at("shift"), 
            *vars.at("low_sigma_gamma_010"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));
    pdfs["gamma_101_shape"] = new RooLITTLEHORNSdini("gamma_101_shape", "", *m_x, 
            *vars.at("low_a_gamma"), 
            *vars.at("low_b_gamma"), 
            *vars.at("low_csi_gamma_101"), 
            *vars.at("shift"), 
            *vars.at("low_sigma_gamma_101"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"), 
            *vars.at("low_shiftg"));
    pdfs["pi_010_shape"] = new RooHORNSdini("pi_010_shape", "", *m_x, 
            *vars.at("low_a_pi"), 
            *vars.at("low_b_pi"), 
            *vars.at("low_csi_pi_010"), 
            *vars.at("shift"), 
            *vars.at("low_sigma_pi_010"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));
    pdfs["pi_101_shape"] = new RooHILLdini("pi_101_shape", "", *m_x, 
            *vars.at("low_a_pi"), 
            *vars.at("low_b_pi"), 
            *vars.at("low_csi_pi_101"), 
            *vars.at("shift"), 
            *vars.at("low_sigma_pi_101"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));

    // Low mass Bs shapes
    pdfs["Bs_gamma_010_shape"] = new RooHILLdini("Bs_gamma_010_shape", "", *m_x, 
            *vars.at("Bs_low_a_gamma"), 
            *vars.at("Bs_low_b_gamma"), 
            *vars.at("Bs_low_csi_gamma_010"), 
            *vars.at("shift"), 
            *vars.at("Bs_low_sigma_gamma_010"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));
    pdfs["Bs_gamma_101_shape"] = new RooLITTLEHORNSdini("Bs_gamma_101_shape", "", 
            *m_x, 
            *vars.at("Bs_low_a_gamma"), 
            *vars.at("Bs_low_b_gamma"), 
            *vars.at("Bs_low_csi_gamma_101"), 
            *vars.at("shift"), 
            *vars.at("Bs_low_sigma_gamma_101"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"), 
            *vars.at("low_shiftg"));
    pdfs["Bs_pi_010_shape"] = new RooHORNSdini("Bs_pi_010_shape", "", *m_x, 
            *vars.at("Bs_low_a_pi"), 
            *vars.at("Bs_low_b_pi"), 
            *vars.at("Bs_low_csi_pi_010"), 
            *vars.at("shift"), 
            *vars.at("Bs_low_sigma_pi_010"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));
    pdfs["Bs_pi_101_shape"] = new RooHILLdini("Bs_pi_101_shape", "", *m_x, 
            *vars.at("Bs_low_a_pi"), 
            *vars.at("Bs_low_b_pi"), 
            *vars.at("Bs_low_csi_pi_101"), 
            *vars.at("shift"), 
            *vars.at("Bs_low_sigma_pi_101"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));

    // Combine helicity components of low mass shapes
    pdfs["low_010_shape"] = new RooAddPdf("low_010_shape", "", 
            RooArgList(*pdfs.at("gamma_010_shape"), *pdfs.at("pi_010_shape")), 
            RooArgList(*vars.at("coeff_gamma_010"), 
                       *vars.at("coeff_pi_010")));
    pdfs["low_101_shape"] = new RooAddPdf("low_101_shape", "", 
            RooArgList(*pdfs.at("gamma_101_shape"), *pdfs.at("pi_101_shape")), 
            RooArgList(*vars.at("coeff_gamma_101"), 
                       *vars.at("coeff_pi_101")));
    pdfs["Bs_low_010_shape"] = new RooAddPdf("Bs_low_010_shape", "", 
            RooArgList(*pdfs.at("Bs_gamma_010_shape"), *pdfs.at("Bs_pi_010_shape")), 
            RooArgList(*vars.at("coeff_gamma_010"), 
                       *vars.at("coeff_pi_010")));
    pdfs["Bs_low_101_shape"] = new RooAddPdf("Bs_low_101_shape", "", 
            RooArgList(*pdfs.at("Bs_gamma_101_shape"), *pdfs.at("Bs_pi_101_shape")), 
            RooArgList(*vars.at("coeff_gamma_101"), 
                       *vars.at("coeff_pi_101")));

    // Combine helicity components
    pdfs["low_Kpi"] = new RooAddPdf("low_Kpi_shape", "", 
            RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
            RooArgList(*vars.at("low_frac_010_Kpi")));
    pdfs["low_piK"] = new RooAddPdf("low_piK_shape", "", 
            RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
            RooArgList(*vars.at("low_frac_010_piK")));
    pdfs["low_KK"] = new RooAddPdf("low_KK_shape", "", 
            RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
            RooArgList(*vars.at("low_frac_010_GLW")));
    pdfs["low_pipi"] = pdfs.at("low_KK");
    pdfs["Bs_low_shape"] = new RooAddPdf("Bs_low_shape", "", 
            RooArgList(*pdfs.at("Bs_low_010_shape"), *pdfs.at("Bs_low_101_shape")), 
            RooArgList(*vars.at("Bs_low_frac_010")));

    // Separate shapes for flavour split fit to account for CP violation
    pdfs["low_Kpi_plus"] = pdfs.at("low_Kpi");
    pdfs["low_Kpi_minus"] = pdfs.at("low_Kpi");
    pdfs["low_piK_plus"] = new RooAddPdf("low_piK_shape_plus", "", 
            RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
            RooArgList(*vars.at("low_frac_010_piK_plus")));
    pdfs["low_piK_minus"] = new RooAddPdf("low_piK_shape_minus", "", 
            RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
            RooArgList(*vars.at("low_frac_010_piK_minus")));
    pdfs["low_KK_plus"] = new RooAddPdf("low_KK_shape_plus", "", 
            RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")),
            RooArgList(*vars.at("low_frac_010_GLW_plus")));
    pdfs["low_KK_minus"] = new RooAddPdf("low_KK_shape_minus", "", 
            RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")),
            RooArgList(*vars.at("low_frac_010_GLW_minus")));
    pdfs["low_pipi_plus"] = pdfs.at("low_KK_plus");
    pdfs["low_pipi_minus"] = pdfs.at("low_KK_minus");

    // Combinatorial shapes
    pdfs["expo_Kpi"] = new RooExponential("expo_Kpi_shape", "", *m_x, 
            *vars.at("slope_Kpi"));
    pdfs["expo_piK"] = pdfs["expo_Kpi"];
    pdfs["expo_KK"] = new RooExponential("expo_KK_shape", "", *m_x, 
            *vars.at("slope_KK"));
    pdfs["expo_pipi"] = new RooExponential("expo_pipi_shape", "", *m_x, 
            *vars.at("slope_pipi"));

    // ===========
    // Observables
    // ===========
    // Make ratios of minus/plus yields from asymmetries
    // Signal
    vars["a_Kpi"] = new RooFormulaVar("a_Kpi", "(1 + @0) / (1 - @0)", 
            RooArgList(*vars.at("A_Kpi")));
    vars["a_KK"] = new RooFormulaVar("a_KK", "(1 + @0) / (1 - @0)", 
            RooArgList(*vars.at("A_KK")));
    vars["a_pipi"] = new RooFormulaVar("a_pipi", "(1 + @0) / (1 - @0)", 
            RooArgList(*vars.at("A_pipi")));

    // Bs 
    vars["a_piK_Bs"] = new RooFormulaVar("a_piK_Bs", "(1 + @0) / (1 - @0)", 
            RooArgList(*vars.at("A_piK_Bs")));
    vars["a_KK_Bs"] = new RooFormulaVar("a_KK_Bs", "(1 + @0) / (1 - @0)", 
            RooArgList(*vars.at("A_KK_Bs")));
    vars["a_pipi_Bs"] = new RooFormulaVar("a_pipi_Bs", "(1 + @0) / (1 - @0)", 
            RooArgList(*vars.at("A_pipi_Bs")));

    // Low mass B0
    vars["a_Kpi_low"] = new RooFormulaVar("a_Kpi_low", "(1 + @0) / (1 - @0)", 
            RooArgList(*vars.at("A_Kpi_low")));
    vars["a_KK_low"] = new RooFormulaVar("a_KK_low", "(1 + @0) / (1 - @0)", 
            RooArgList(*vars.at("A_KK_low")));
    vars["a_pipi_low"] = new RooFormulaVar("a_pipi_low", "(1 + @0) / (1 - @0)", 
            RooArgList(*vars.at("A_pipi_low")));

    // ======
    // Yields
    // ======
    // Total signal yields
    vars["n_signal_piK"] = new RooFormulaVar("n_signal_piK", "@0 * @1", 
            RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_piK_vs_Kpi")));
    vars["n_signal_KK"] = new RooFormulaVar("n_signal_KK", "@0 * @1", 
            RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_KK_vs_Kpi")));
    vars["n_signal_pipi"] = new RooFormulaVar("n_signal_pipi", "@0 * @1", 
            RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_pipi_vs_Kpi")));

    // Flavour split signal yields
    vars["n_signal_Kpi_plus"] = new RooFormulaVar("n_signal_Kpi_plus", 
            "@0 / (1 + @1)", 
            RooArgList(*vars.at("n_signal_Kpi"), *vars.at("a_Kpi"))); 
    vars["n_signal_Kpi_minus"] = new RooFormulaVar("n_signal_Kpi_minus", 
            "@0 / (1 + 1/@1)", 
            RooArgList(*vars.at("n_signal_Kpi"), *vars.at("a_Kpi"))); 
    vars["n_signal_KK_plus"] = new RooFormulaVar("n_signal_KK_plus", 
            "@0 / (1 + @1)", 
            RooArgList(*vars.at("n_signal_KK"), *vars.at("a_KK"))); 
    vars["n_signal_KK_minus"] = new RooFormulaVar("n_signal_KK_minus", 
            "@0 / (1 + 1/@1)", 
            RooArgList(*vars.at("n_signal_KK"), *vars.at("a_KK"))); 
    vars["n_signal_pipi_plus"] = new RooFormulaVar("n_signal_pipi_plus", 
            "@0 / (1 + @1)", 
            RooArgList(*vars.at("n_signal_pipi"), *vars.at("a_pipi"))); 
    vars["n_signal_pipi_minus"] = new RooFormulaVar("n_signal_pipi_minus", 
            "@0 / (1 + 1/@1)", 
            RooArgList(*vars.at("n_signal_pipi"), *vars.at("a_pipi"))); 

    // Make suppressed mode yields from CP ratios
    vars["n_signal_piK_plus"] = new RooFormulaVar("n_signal_piK_plus", 
            "@0 * @1", 
            RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_plus")));
    vars["n_signal_piK_minus"] = new RooFormulaVar("n_signal_piK_minus", "@0 * @1", 
            RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_minus")));

    // Bs yields
    vars["n_Bs_KK"] = new RooFormulaVar("n_Bs_KK", "@0 * @1", 
            RooArgList(*vars.at("n_Bs_piK"), *vars.at("R_KK_vs_piK_Bs")));
    vars["n_Bs_pipi"] = new RooFormulaVar("n_Bs_pipi", "@0 * @1", 
            RooArgList(*vars.at("n_Bs_piK"), *vars.at("R_pipi_vs_piK_Bs")));
    vars["n_Bs_piK_plus"] = new RooFormulaVar("n_Bs_piK_plus", 
            "@0 / (1 + @1)", 
            RooArgList(*vars.at("n_Bs_piK"), *vars.at("a_piK_Bs"))); 
    vars["n_Bs_piK_minus"] = new RooFormulaVar("n_Bs_piK_minus", 
            "@0 / (1 + 1/@1)", 
            RooArgList(*vars.at("n_Bs_piK"), *vars.at("a_piK_Bs"))); 
    vars["n_Bs_KK_plus"] = new RooFormulaVar("n_Bs_KK_plus", 
            "@0 / (1 + @1)", 
            RooArgList(*vars.at("n_Bs_KK"), *vars.at("a_KK_Bs"))); 
    vars["n_Bs_KK_minus"] = new RooFormulaVar("n_Bs_KK_minus", 
            "@0 / (1 + 1/@1)", 
            RooArgList(*vars.at("n_Bs_KK"), *vars.at("a_KK_Bs"))); 
    vars["n_Bs_pipi_plus"] = new RooFormulaVar("n_Bs_pipi_plus", 
            "@0 / (1 + @1)", 
            RooArgList(*vars.at("n_Bs_pipi"), *vars.at("a_pipi_Bs"))); 
    vars["n_Bs_pipi_minus"] = new RooFormulaVar("n_Bs_pipi_minus", 
            "@0 / (1 + 1/@1)", 
            RooArgList(*vars.at("n_Bs_pipi"), *vars.at("a_pipi_Bs"))); 

    // Low mass yields
    vars["n_low_piK"] = new RooFormulaVar("n_low_piK", "@0 * @1", 
            RooArgList(*vars.at("n_low_Kpi"), *vars.at("R_piK_vs_Kpi_low")));
    vars["n_low_KK"] = new RooFormulaVar("n_low_KK", "@0 * @1", 
            RooArgList(*vars.at("n_low_Kpi"), *vars.at("R_KK_vs_Kpi_low")));
    vars["n_low_pipi"] = new RooFormulaVar("n_low_pipi", "@0 * @1", 
            RooArgList(*vars.at("n_low_Kpi"), *vars.at("R_pipi_vs_Kpi_low")));

    // Flavour split low mass yields
    vars["n_low_Kpi_plus"] = new RooFormulaVar("n_low_Kpi_plus", "@0 / (1 + @1)", 
            RooArgList(*vars.at("n_low_Kpi"), *vars.at("a_Kpi_low")));
    vars["n_low_Kpi_minus"] = new RooFormulaVar("n_low_Kpi_minus", 
            "@0 / (1 + 1/@1)", 
            RooArgList(*vars.at("n_low_Kpi"), *vars.at("a_Kpi_low")));
    vars["n_low_KK_plus"] = new RooFormulaVar("n_low_KK_plus", "@0 / (1 + @1)", 
            RooArgList(*vars.at("n_low_KK"), *vars.at("a_KK_low")));
    vars["n_low_KK_minus"] = new RooFormulaVar("n_low_KK_minus", 
            "@0 / (1 + 1/@1)", 
            RooArgList(*vars.at("n_low_KK"), *vars.at("a_KK_low")));
    vars["n_low_pipi_plus"] = new RooFormulaVar("n_low_pipi_plus", 
            "@0 / (1 + @1)", 
            RooArgList(*vars.at("n_low_pipi"), *vars.at("a_pipi_low")));
    vars["n_low_pipi_minus"] = new RooFormulaVar("n_low_pipi_minus", 
            "@0 / (1 + 1/@1)", 
            RooArgList(*vars.at("n_low_pipi"), *vars.at("a_pipi_low")));

    // Split piK low mass yields using ratios
    vars["n_low_piK_plus"] = new RooFormulaVar("n_low_piK_plus", "@0 * @1", 
            RooArgList(*vars.at("n_low_Kpi_plus"), *vars.at("R_plus_low")));
    vars["n_low_piK_minus"] = new RooFormulaVar("n_low_piK_minus", 
            "@0 * @1", 
            RooArgList(*vars.at("n_low_Kpi_minus"), *vars.at("R_minus_low")));

    // Bs low mass yields
    vars["n_Bs_low_KK"] = new RooFormulaVar("n_Bs_low_KK", "@0 * @1", 
            RooArgList(*vars.at("n_Bs_low_piK"), 
                       *vars.at("R_KK_vs_piK_Bs_low")));
    vars["n_Bs_low_pipi"] = new RooFormulaVar("n_Bs_low_pipi", "@0 * @1", 
            RooArgList(*vars.at("n_Bs_low_piK"), 
                       *vars.at("R_pipi_vs_piK_Bs_low")));

    // Flavour split Bs low mass yields (no asymmetry, shared equally)
    vars["n_Bs_low_piK_plus"] = new RooFormulaVar("n_Bs_low_piK_plus", 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_piK")));
    vars["n_Bs_low_piK_minus"] = new RooFormulaVar("n_Bs_low_piK_minus", 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_piK")));
    vars["n_Bs_low_KK_plus"] = new RooFormulaVar("n_Bs_low_KK_plus", 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_KK")));
    vars["n_Bs_low_KK_minus"] = new RooFormulaVar("n_Bs_low_KK_minus", 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_KK")));
    vars["n_Bs_low_pipi_plus"] = new RooFormulaVar("n_Bs_low_pipi_plus", 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_pipi")));
    vars["n_Bs_low_pipi_minus"] = new RooFormulaVar("n_Bs_low_pipi_minus", 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_pipi")));

    // Rho yields
    vars["n_rho_piK"] = vars["n_rho_Kpi"];
    vars["n_rho_KK"] = new RooFormulaVar("n_rho_KK", "@0 * @1", 
            RooArgList(*vars.at("n_rho_piK"), *vars.at("R_KK_vs_piK_Bs")));
    vars["n_rho_pipi"] = new RooFormulaVar("n_rho_pipi", "@0 * @1", 
            RooArgList(*vars.at("n_rho_piK"), *vars.at("R_pipi_vs_piK_Bs")));

    // Flavour split rho yields (split equally)
    vars["n_rho_Kpi_plus"] = new RooFormulaVar("n_rho_Kpi_plus", "@0 / 2", 
            RooArgList(*vars.at("n_rho_Kpi")));
    vars["n_rho_Kpi_minus"] = new RooFormulaVar("n_rho_Kpi_minus", "@0 / 2", 
            RooArgList(*vars.at("n_rho_Kpi")));
    vars["n_rho_piK_plus"] = new RooFormulaVar("n_rho_piK_plus", "@0 / 2", 
            RooArgList(*vars.at("n_rho_piK")));
    vars["n_rho_piK_minus"] = new RooFormulaVar("n_rho_piK_minus", "@0 / 2", 
            RooArgList(*vars.at("n_rho_piK")));
    vars["n_rho_KK_plus"] = new RooFormulaVar("n_rho_KK_plus", "@0 / 2", 
            RooArgList(*vars.at("n_rho_KK")));
    vars["n_rho_KK_minus"] = new RooFormulaVar("n_rho_KK_minus", "@0 / 2", 
            RooArgList(*vars.at("n_rho_KK")));
    vars["n_rho_pipi_plus"] = new RooFormulaVar("n_rho_pipi_plus", "@0 / 2", 
            RooArgList(*vars.at("n_rho_pipi")));
    vars["n_rho_pipi_minus"] = new RooFormulaVar("n_rho_pipi_minus", "@0 / 2", 
            RooArgList(*vars.at("n_rho_pipi")));

    // Flavour split exponential yields (split equally)
    vars["n_expo_Kpi_plus"] = new RooFormulaVar("n_expo_Kpi_plus", "@0 / 2", 
            RooArgList(*vars.at("n_expo_Kpi")));
    vars["n_expo_Kpi_minus"] = new RooFormulaVar("n_expo_Kpi_minus", "@0 / 2", 
            RooArgList(*vars.at("n_expo_Kpi")));
    vars["n_expo_piK_plus"] = new RooFormulaVar("n_expo_piK_plus", "@0 / 2", 
            RooArgList(*vars.at("n_expo_piK")));
    vars["n_expo_piK_minus"] = new RooFormulaVar("n_expo_piK_minus", "@0 / 2", 
            RooArgList(*vars.at("n_expo_piK")));
    vars["n_expo_KK_plus"] = new RooFormulaVar("n_expo_KK_plus", "@0 / 2", 
            RooArgList(*vars.at("n_expo_KK")));
    vars["n_expo_KK_minus"] = new RooFormulaVar("n_expo_KK_minus", "@0 / 2", 
            RooArgList(*vars.at("n_expo_KK")));
    vars["n_expo_pipi_plus"] = new RooFormulaVar("n_expo_pipi_plus", "@0 / 2", 
            RooArgList(*vars.at("n_expo_pipi")));
    vars["n_expo_pipi_minus"] = new RooFormulaVar("n_expo_pipi_minus", "@0 / 2", 
            RooArgList(*vars.at("n_expo_pipi")));

    // ===================
    // Make each fit shape
    // ===================
    for (auto mode : m_modes) {
        if (m_sum == "Y") {
            // Lists of shapes and yields
            RooArgList shapes_list;
            RooArgList yields_list;

            // Add shapes
            shapes_list.add(*pdfs.at("signal_shape"));
            shapes_list.add(*pdfs.at("expo_" + mode));
            shapes_list.add(*pdfs.at("low_" + mode));
            shapes_list.add(*pdfs.at("rho_shape"));
            if (mode != "Kpi") {
                shapes_list.add(*pdfs.at("Bs_shape"));
                shapes_list.add(*pdfs.at("Bs_low_shape"));
            }

            // Add yields
            yields_list.add(*vars.at("n_signal_" + mode));
            yields_list.add(*vars.at("n_expo_" + mode));
            yields_list.add(*vars.at("n_low_" + mode));
            yields_list.add(*vars.at("n_rho_" + mode));
            if (mode != "Kpi") {
                yields_list.add(*vars.at("n_Bs_" + mode));
                yields_list.add(*vars.at("n_Bs_low_" + mode));
            }

            // Make shape
            pdfs["fit_" + mode] = new RooAddPdf((mode + "_fit").c_str(), "", 
                    shapes_list, yields_list);

        } else {

            // List of shapes and yields
            std::cout << "Making fit shape for " << mode << std::endl;
            RooArgList shapes_list_plus;
            RooArgList shapes_list_minus;
            RooArgList yields_list_plus;
            RooArgList yields_list_minus;

            // Add shapes
            shapes_list_plus.add(*pdfs.at("signal_shape"));
            shapes_list_minus.add(*pdfs.at("signal_shape"));
            shapes_list_plus.add(*pdfs.at("expo_" + mode));
            shapes_list_minus.add(*pdfs.at("expo_" + mode));
            shapes_list_plus.add(*pdfs.at("low_" + mode + "_plus"));
            shapes_list_minus.add(*pdfs.at("low_" + mode + "_minus"));
            shapes_list_plus.add(*pdfs.at("rho_shape"));
            shapes_list_minus.add(*pdfs.at("rho_shape"));
            if (mode != "Kpi") {
                shapes_list_plus.add(*pdfs.at("Bs_shape"));
                shapes_list_minus.add(*pdfs.at("Bs_shape"));
                shapes_list_plus.add(*pdfs.at("Bs_low_shape"));
                shapes_list_minus.add(*pdfs.at("Bs_low_shape"));
            }

            // Add yields
            yields_list_plus.add(*vars.at("n_signal_" + mode + "_plus"));
            yields_list_minus.add(*vars.at("n_signal_" + mode + "_minus"));
            yields_list_plus.add(*vars.at("n_expo_" + mode + "_plus"));
            yields_list_minus.add(*vars.at("n_expo_" + mode + "_minus"));
            yields_list_plus.add(*vars.at("n_low_" + mode + "_plus"));
            yields_list_minus.add(*vars.at("n_low_" + mode + "_minus"));
            yields_list_plus.add(*vars.at("n_rho_" + mode + "_plus"));
            yields_list_minus.add(*vars.at("n_rho_" + mode + "_minus"));
            if (mode != "Kpi") {
                yields_list_plus.add(*vars.at("n_Bs_" + mode + "_plus"));
                yields_list_minus.add(*vars.at("n_Bs_" + mode + "_minus"));
                yields_list_plus.add(*vars.at("n_Bs_low_" + mode + "_plus"));
                yields_list_minus.add(*vars.at("n_Bs_low_" + mode + "_minus"));
            }

            // Print lists
            yields_list_plus.Print();
            yields_list_minus.Print();
            shapes_list_plus.Print();
            shapes_list_minus.Print();

            // Make shapes
            pdfs["fit_" + mode + "_plus"] = new RooAddPdf(
                    (mode + "_fit_plus").c_str(), "", 
                    shapes_list_plus, yields_list_plus);
            pdfs["fit_" + mode + "_minus"] = new RooAddPdf(
                    (mode + "_fit_minus").c_str(), "", 
                    shapes_list_minus, yields_list_minus);
        }
    }

    // Combine PDFs
    RooSimultaneous * simPdf = new RooSimultaneous("simPdf", "", *m_cat);
    for (auto mode : m_modes) {
        if (m_sum == "Y") {
            simPdf->addPdf(*pdfs.at(mode + "_fit"), mode.c_str());
        } else {
            simPdf->addPdf(*pdfs.at("fit_" + mode + "_plus"), 
                    (mode + "_plus").c_str());
            simPdf->addPdf(*pdfs.at("fit_" + mode + "_minus"), 
                    (mode + "_minus").c_str());
        }
    }
    simPdf->Print("v");
    return simPdf;
}


// =============================================
// Function to save PDFs as histograms in a file
// =============================================
void ShapeMaker::saveFitHistograms(std::string filename, 
        std::map<std::string, RooDataSet*> dataMap) {

    // Open the file
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");

    // Loop through modes
    for (auto mode : m_modes) {

        // Categories to use for each mode
        std::vector<std::string> mode_categories;
        if (m_sum == "Y") {
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

            // Binning of data
            int nBins = m_x->getBinning().numBins();

            // Convert data and fit to TH1Fs
            TH1F * h_data = (TH1F*)dataMap[fullname]->createHistogram(
                    ("h_data_" + fullname).c_str(), *m_x);
            TH1F * h_fit = (TH1F*)m_fit_pdfs.at("fit_" + fullname)->createHistogram(
                    ("h_fit_" + fullname).c_str(), *m_x, 
                    RooFit::Binning(nBins * 10));

            // Convert fit components
            TH1F * h_signal = (TH1F*)m_fit_pdfs.at("signal_shape")->createHistogram(
                    ("h_signal_" + fullname).c_str(), *m_x, 
                    RooFit::Binning(nBins * 10));
            TH1F * h_expo = (TH1F*)m_fit_pdfs.at("expo_" + mode)->createHistogram(
                    ("h_expo_" + fullname).c_str(), *m_x, 
                    RooFit::Binning(nBins * 10));
            TH1F * h_low = (TH1F*)m_fit_pdfs.at("low_" + fullname)->createHistogram(
                    ("h_low_" + fullname).c_str(), *m_x, 
                    RooFit::Binning(nBins * 10));
            TH1F * h_rho = (TH1F*)m_fit_pdfs.at("rho_shape")->createHistogram(
                    ("h_rho_" + fullname).c_str(), *m_x, 
                    RooFit::Binning(nBins * 10)); 

            // Scale histograms
            h_fit->Scale(h_data->Integral() * 10 / h_fit->Integral());
            h_signal->Scale(m_fit_vars.at("n_signal_" + fullname)->getVal() * 10 / 
                    h_signal->Integral());
            h_expo->Scale(m_fit_vars.at("n_expo_" + fullname)->getVal() * 10 / 
                    h_expo->Integral());
            h_low->Scale(m_fit_vars.at("n_low_" + fullname)->getVal() * 10 / 
                    h_low->Integral());
            h_rho->Scale(m_fit_vars.at("n_rho_" + fullname)->getVal() * 10 / 
                    h_rho->Integral());

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
                RooPlot * frame = m_x->frame();
                dataMap[fullname]->plotOn(frame);
                m_fit_pdfs.at("fit_" + fullname)->plotOn(frame);
                RooHist * pulls = frame->pullHist();
                pulls->Write(("pulls_" + fullname).c_str());

            } else {

                // Make Bs histograms
                TH1F * h_Bs = (TH1F*)m_fit_pdfs.at("Bs_shape")->createHistogram(
                        ("h_Bs_" + fullname).c_str(), *m_x, 
                        RooFit::Binning(nBins * 10));
                TH1F * h_Bs_low = (TH1F*)m_fit_pdfs.at("Bs_low_shape")->createHistogram(
                        ("h_Bs_low_" + fullname).c_str(), *m_x, 
                        RooFit::Binning(nBins * 10));

                // Scale Bs histograms
                h_Bs->Scale(m_fit_vars.at("n_Bs_" + fullname)->getVal() * 10 / 
                        h_Bs->Integral());
                h_Bs_low->Scale(m_fit_vars.at("n_Bs_low_" + fullname)->getVal() * 10 / 
                        h_Bs_low->Integral());

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
                    if (bin_val > lsb && bin_val < usb) {
                        h_data_blind->SetBinContent(bin, 0);
                    }
                    if (bin_val > lsb && bin_val < usb) { 
                        h_data_blind->SetBinError(bin, 0);
                    }
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
                RooPlot * frame = m_x->frame();
                dataMap[fullname]->plotOn(frame);
                m_fit_pdfs.at("fit_" + fullname)->plotOn(frame);
                RooHist * pulls = frame->pullHist();
                pulls->Write(("pulls_" + fullname).c_str());
            }
        }
    }
    outfile->Close();
}


// =================
// Read RooFitResult
// =================
std::map<std::string, double> * ShapeMaker::readFitResult(std::string results_file) 
{

    // Open the file
    TFile * res_file = TFile::Open(results_file.c_str(), "READ");
    RooFitResult * res = (RooFitResult*)res_file->Get("fit_result");

    // Make map for results
    std::map<std::string, double> * res_map = new std::map<std::string, double>();

    // Iterate through result and get variables
    RooArgList vars = res->floatParsFinal();
    RooRealVar * var;
    TIterator * it = vars.createIterator();
    while ((var = (RooRealVar*)it->Next())) {
        res_map->emplace(var->GetName(), var->getVal());
    }
    return res_map;
}
