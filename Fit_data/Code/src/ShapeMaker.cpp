#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TRandom.h"

#include "RooAddPdf.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooHist.h"
#include "RooArgList.h"
#include "RooArgList.h"
#include "RooDataSet.h"
#include "RooCategory.h"
#include "RooCBShape.h"
#include "RooFormulaVar.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooExponential.h"
#include "RooUnblindUniform.h"
#include "RooRandom.h"

#include "RooHILLdini.h"
#include "RooHORNSdini.h"
#include "RooLITTLEHORNSdini.h"

#include "ParameterReader.hpp"
#include "ShapeMaker.hpp"


// ==================================================
// Constructor with default no inclusion of four-body
// ==================================================
ShapeMaker::ShapeMaker(std::string sum, RooRealVar * Bd_M) : 
    m_sum((sum == "Y")), m_x(Bd_M), m_fourBody(false) {
        setup();
}

// ===========
// Constructor
// ===========
ShapeMaker::ShapeMaker(std::string sum, RooRealVar * Bd_M, bool fourBody) : 
    m_sum((sum == "Y")), m_x(Bd_M), m_fourBody(fourBody) {
        setup();
}


// ============
// Set up class
// ============
void ShapeMaker::setup() {

    // List of modes
    m_modes = {"Kpi", "piK", "KK", "pipi"};
    m_4_modes = {"Kpipipi", "piKpipi", "pipipipi"};

    // Set up category    
    m_cat = new RooCategory("category", "category");
    for (auto mode : m_modes) {
        if (m_sum) {
            m_cat->defineType(mode.c_str());
        } else {
            m_cat->defineType((mode + "_plus").c_str());
            m_cat->defineType((mode + "_minus").c_str());
        }
    }

    // Add four-body to catgeory
    if (m_fourBody) {
        for (auto mode : m_4_modes) {
            if (m_sum) {
                m_cat->defineType(mode.c_str());
            } else {
                m_cat->defineType((mode + "_plus").c_str());
                m_cat->defineType((mode + "_minus").c_str());
            }
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
    
    // Read four-body fit parameters
    if (m_fourBody) {
        m_pr->readParams("4_signal", MC_path + "signal_Kpipipi.param");
    }

    // Flag for whether map of yields is filled
    m_yieldsCalculated = false;

    // Set two-body constants now so they can be shifted before shape creation
    setTwoBodyConstants(m_fit_vars, false);

}


// ==========
// Destructor
// ==========
ShapeMaker::~ShapeMaker() {
    delete m_pr;
}



// ======================================================
// Make PDF to use with data fit (pass in maximum yields)
// ======================================================
RooSimultaneous * ShapeMaker::makeFitPdf(const YieldMap & max_yields, bool blind) {
    return makeFitPdf(max_yields, blind, false);
}
    

// ===========================================================
// Make PDF to use in fitting with yields from a generated toy
// ===========================================================
RooSimultaneous * ShapeMaker::makeFitPdf(bool blind) {
    
    // Check map is initialised
    if (!m_yieldsCalculated) {
        std::cout << "Error: yield map has not been filled! " <<
            "ShapeMaker::makeGenerationPdf() needs to be called first."
            << std::endl;
        exit(EXIT_FAILURE);
    }

    // Use normal function with m_expectedYields
    return makeFitPdf(m_expectedYields, blind, false);
}


// ==============================================================
// Make PDF to use in fitting a toy, with piK yield fixed to zero
// ==============================================================
RooSimultaneous * ShapeMaker::makeZeroYieldPdf() {
    
    // Check map is initialised
    if (!m_yieldsCalculated) {
        std::cout << "Error: yield map has not been filled! " <<
            "ShapeMaker::makeGenerationPdf() needs to be called first."
            << std::endl;
        exit(EXIT_FAILURE);
    }

    // Use normal function with m_expectedYields
    return makeFitPdf(m_expectedYields, false, true);
}


// ============================
// Make a PDF to use in fitting
// ============================
RooSimultaneous * ShapeMaker::makeFitPdf(const YieldMap & max_yields, bool blind,
        bool zero_piK_yield) {

    std::cout << "Making a fit PDF" << std::endl;
    // ====================
    // Set up floating vars
    // ====================
    // Shifts
    m_fit_vars["shift"] = new RooRealVar("shift", "", 0, -10, 10);

    // Signal parameters
    m_fit_vars["signal_sigma_L"] = new RooRealVar("signal_sigma_L", "", 
            m_pr->getParam("signal_sigma_L"), 
            m_pr->getParam("signal_sigma_L") - 10, 
            m_pr->getParam("signal_sigma_L") + 10);

    // Bs floating parameters
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
    if (blind) {
        m_fit_vars["A_KK_blind"] = new RooRealVar("A_KK_blind", "", 0, -1, 1);
        m_fit_vars["A_pipi_blind"] = new RooRealVar("A_pipi_blind", "", 0, -1, 1);
        m_fit_vars["A_KK"] = new RooUnblindUniform("A_KK", "", "blind_KK_asym", 0.01, 
                *m_fit_vars.at("A_KK_blind"));
        m_fit_vars["A_pipi"] = new RooUnblindUniform("A_pipi", "", 
                "blind_pipi_asym", 0.01, *m_fit_vars.at("A_pipi_blind"));
    } else {
        m_fit_vars["A_KK"] = new RooRealVar("A_KK", 0, -1, 1);
        m_fit_vars["A_pipi"] = new RooRealVar("A_pipi", 0, -1, 1);
    }

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
    if (blind) {
        m_fit_vars["R_piK_vs_Kpi_blind"] = new RooRealVar("R_piK_vs_Kpi_blind", "", 
                0.06, 0, 0.3);
        m_fit_vars["R_piK_vs_Kpi"] = new RooUnblindUniform("R_piK_vs_Kpi", "", 
                "blind_piK_ratio", 0.01, *m_fit_vars.at("R_piK_vs_Kpi_blind"));
    } else {
        if (zero_piK_yield) {
            m_fit_vars["R_piK_vs_Kpi"] = new RooRealVar("R_piK_vs_Kpi", "", 0);
        } else {
            m_fit_vars["R_piK_vs_Kpi"] = new RooRealVar("R_piK_vs_Kpi", "", 
                    0.06, 0, 1);
        }
    }

    // CP modes to favoured ADS ratio
    if (blind) {
        m_fit_vars["R_KK_vs_Kpi_blind"] = new RooRealVar("R_KK_vs_Kpi_blind", "", 
                0.3, 0, 1);
        m_fit_vars["R_KK_vs_Kpi"] = new RooUnblindUniform("R_KK_vs_Kpi", "", 
                "blind_KK_ratio", 0.1, *m_fit_vars.at("R_KK_vs_Kpi_blind"));
        m_fit_vars["R_pipi_vs_Kpi_blind"] = new RooRealVar("R_pipi_vs_Kpi_blind", "", 
                0.1, 0, 1);
        m_fit_vars["R_pipi_vs_Kpi"] = new RooUnblindUniform("R_pipi_vs_Kpi", "", 
                "blind_pipi_ratio", 0.05, *m_fit_vars.at("R_pipi_vs_Kpi_blind"));
    } else {
        m_fit_vars["R_KK_vs_Kpi"] = new RooRealVar("R_KK_vs_Kpi", "", 
                0.3, 0, 1);
        m_fit_vars["R_pipi_vs_Kpi"] = new RooRealVar("R_pipi_vs_Kpi", "", 
                0.1, 0, 1);
    }

    // Flavour split ratios
    if (blind) {
        m_fit_vars["R_plus_blind"] = new RooRealVar("R_plus_blind", "", 0.06, 0, 1);
        m_fit_vars["R_minus_blind"] = new RooRealVar("R_minus_blind", "", 0.06, 0, 1);
        m_fit_vars["R_plus"] = new RooUnblindUniform("R_plus", "", "blind_R_plus", 
                0.01, *m_fit_vars.at("R_plus_blind"));
        m_fit_vars["R_minus"] = new RooUnblindUniform("R_minus", "", "blind_R_minus", 
                0.01, *m_fit_vars.at("R_minus_blind"));
    } else {
        if (zero_piK_yield) {
            m_fit_vars["R_plus"] = new RooRealVar("R_plus", "", 0);
            m_fit_vars["R_minus"] = new RooRealVar("R_minus", "", 0);
        } else {
            m_fit_vars["R_plus"] = new RooRealVar("R_plus", "", 0.06, 0, 1);
            m_fit_vars["R_minus"] = new RooRealVar("R_minus", "", 0.06, 0, 1);
        }
    }

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

    // Low mass Bs ratios : fixed for now
    // m_fit_vars["R_KK_vs_piK_Bs_low"] = new RooRealVar("R_KK_vs_piK_Bs_low", "",
            // 0.3, 0, 1);
    // m_fit_vars["R_pipi_vs_piK_Bs_low"] = new RooRealVar("R_pipi_vs_piK_Bs_low", "",
            // 0.1, 0, 1);

    // ====================
    // Make floating yields
    // ====================
    m_fit_vars["n_signal_Kpi"] = new RooRealVar("n_signal_Kpi", "", 100, 0, 
            max_yields.at("Kpi"));
    m_fit_vars["n_Bs_piK"] = new RooRealVar("n_Bs_piK", "", 100, 0, 
            max_yields.at("piK"));
    m_fit_vars["n_low_Kpi"] = new RooRealVar("n_low_Kpi", "", 100, 0, 
            max_yields.at("Kpi"));
    m_fit_vars["n_Bs_low_piK"] = new RooRealVar("n_Bs_low_piK", "", 100, 0, 
            max_yields.at("piK"));
    m_fit_vars["n_rho_Kpi"] = new RooRealVar("n_rho_Kpi", "", 20, 0, 
            max_yields.at("Kpi")/20);
    for (auto mode : m_modes) {
        m_fit_vars["n_expo_" + mode] = new RooRealVar(("n_expo_" + mode).c_str(), 
                "", max_yields.at(mode)/2 , 0, max_yields.at(mode));
    }

    // Make four-body floating variables if needed
    if (m_fourBody) makeFloatingFourBodyVars(max_yields, blind);

    // Return simultaneous PDF
    return makePdf(m_fit_vars, m_fit_pdfs, false);
}


// ==================================
// Make floating four-body parameters
// ==================================
void ShapeMaker::makeFloatingFourBodyVars(const YieldMap & max_yields, bool blind) {

    // Signal parameters
    m_fit_vars["4_signal_sigma_L"] = new RooRealVar("4_signal_sigma_L", "", 
            m_pr->getParam("4_signal_sigma_L"), 
            m_pr->getParam("4_signal_sigma_L") - 10, 
            m_pr->getParam("4_signal_sigma_L") + 10);

    // Bs floating parameters
    m_fit_vars["4_Bs_sigma_L"] = new RooRealVar("4_Bs_sigma_L", "", 
            m_pr->getParam("Bs_sigma_L"), 
            m_pr->getParam("Bs_sigma_L") - 10, 
            m_pr->getParam("Bs_sigma_L") + 10);

    // Floating fraction of 010 shape
    m_fit_vars["4_low_frac_010_Kpi"] = new RooRealVar("4_low_frac_010_Kpi", "", 
            0.5, 0, 1);
    m_fit_vars["4_low_frac_010_piK"] = new RooRealVar("4_low_frac_010_piK", "", 
            0.5, 0, 1);
    m_fit_vars["4_low_frac_010_piK_plus"] = new 
        RooRealVar("4_low_frac_010_piK_plus", "", 0.5, 0, 1);
    m_fit_vars["4_low_frac_010_piK_minus"] = new 
        RooRealVar("4_low_frac_010_piK_minus", "", 0.5, 0, 1);
    m_fit_vars["4_low_frac_010_GLW"] = new RooRealVar("4_low_frac_010_GLW", "", 
            0.5, 0, 1);
    m_fit_vars["4_low_frac_010_GLW_plus"] = new 
        RooRealVar("4_low_frac_010_GLW_plus", "", 0.5, 0, 1);
    m_fit_vars["4_low_frac_010_GLW_minus"] = new RooRealVar(
            "4_low_frac_010_GLW_minus", "", 0.5, 0, 1);
    // Bs low fraction is shared with two-body

    // Slope of exponentials
    m_fit_vars["slope_Kpipipi"] = new RooRealVar("slope_Kpipipi", "", -0.005, -0.5, 0.0);
    m_fit_vars["slope_pipipipi"] = new RooRealVar("slope_pipipipi", "", -0.005, -0.5, 0.0);

    // ===========================
    // Set up floating observables
    // ===========================
    // Signal asymmetries
    m_fit_vars["A_Kpipipi"] = new RooRealVar("A_Kpipipi", "", 0, -1, 1);

    // KK and pipi blind asymmetries
    if (blind) {
        m_fit_vars["A_pipipipi_blind"] = new RooRealVar("A_pipipipi_blind", "", 
                0, -1, 1);
        m_fit_vars["A_pipipipi"] = new RooUnblindUniform("A_pipipipi", "", 
                "blind_pipipipi_asym", 0.01, *m_fit_vars.at("A_pipipipi_blind"));
    } else {
        m_fit_vars["A_pipipipi"] = new RooRealVar("A_pipipipi", 0, -1, 1);
    }

    // Bs asymmetries (check these for signs etc later)
    m_fit_vars["A_piKpipi_Bs"] = new RooRealVar("A_piKpipi_Bs", "", 0, -1, 1);
    m_fit_vars["A_pipipipi_Bs"] = new RooRealVar("A_pipipipi_Bs", "", 0, -1, 1);

    // Low mass asymmetries
    m_fit_vars["A_Kpipipi_low"] = new RooRealVar("A_Kpipipi_low", "", 0, -1, 1);
    m_fit_vars["A_pipipipi_low"] = new RooRealVar("A_pipipipi_low", "", 0, -1, 1);

    // Signal ratios
    // Suppressed to favoured ADS ratio
    if (blind) {
        m_fit_vars["R_piKpipi_vs_Kpipipi_blind"] = new 
            RooRealVar("R_piKpipi_vs_Kpi_blind", "", 0.06, 0, 0.3);
        m_fit_vars["R_piKpipi_vs_Kpipipi"] = new 
            RooUnblindUniform("R_piKpipi_vs_Kpipipi", "", 
                    "blind_piKpipi_ratio", 0.01, 
                    *m_fit_vars.at("R_piKpipi_vs_Kpipipi_blind"));
    } else {
        m_fit_vars["R_piKpipi_vs_Kpipipi"] = new 
            RooRealVar("R_piKpipi_vs_Kpipipi", "", 0.06, 0, 1);
    }

    // CP modes to favoured ADS ratio
    if (blind) {
        m_fit_vars["R_pipipipi_vs_Kpipipi_blind"] = new 
            RooRealVar("R_pipipipi_vs_Kpipipi_blind", "", 0.1, 0, 1);
        m_fit_vars["R_pipipipi_vs_Kpipipi"] = new 
            RooUnblindUniform("R_pipipipi_vs_Kpipipi", "", "blind_pipipipi_ratio", 
                    0.05, *m_fit_vars.at("R_pipipipi_vs_Kpipipi_blind"));
    } else {
        m_fit_vars["R_pipipipi_vs_Kpipipi"] = new 
            RooRealVar("R_pipipipi_vs_Kpipipi", "", 0.1, 0, 1);
    }

    // Flavour split ratios
    if (blind) {
        m_fit_vars["4_R_plus_blind"] = new 
            RooRealVar("4_R_plus_blind", "", 0.06, 0, 1);
        m_fit_vars["4_R_minus_blind"] = new 
            RooRealVar("4_R_minus_blind", "", 0.06, 0, 1);
        m_fit_vars["4_R_plus"] = new 
            RooUnblindUniform("4_R_plus", "", "blind_R_plus4_", 
                    0.01, *m_fit_vars.at("4_R_plus_blind"));
        m_fit_vars["4_R_minus"] = new RooUnblindUniform("4_R_minus", "", 
                "blind_R_minus_4", 0.01, *m_fit_vars.at("4_R_minus_blind"));
    } else {
        m_fit_vars["4_R_plus"] = new RooRealVar("4_R_plus", "", 0.06, 0, 1);
        m_fit_vars["4_R_minus"] = new RooRealVar("4_R_minus", "", 0.06, 0, 1);
    }

    // Bs ratios
    m_fit_vars["R_pipipipi_vs_piKpipi_Bs"] = new 
        RooRealVar("R_pipipipi_vs_piKpipi_Bs", "", 0.1, 0, 1);

    // Low mass ratios
    m_fit_vars["R_piKpipi_vs_Kpipipi_low"] = new 
        RooRealVar("R_piKpipi_vs_Kpipipi_low", "", 0.06, 0, 1);
    m_fit_vars["R_pipipipi_vs_Kpipipi_low"] = new 
        RooRealVar("R_pipipipi_vs_Kpipipi_low", "", 0.1, 0, 1);

    // Low mass flavour split ratios
    m_fit_vars["4_R_plus_low"] = new RooRealVar("4_R_plus_low", "", 0.06, 0, 1);
    m_fit_vars["4_R_minus_low"] = new RooRealVar("4_R_minus_low", "", 0.06, 0, 1);

    // ====================
    // Make floating yields
    // ====================
    m_fit_vars["n_signal_Kpipipi"] = new RooRealVar("n_signal_Kpipipi", "", 100, 0, 
            max_yields.at("Kpipipi"));
    m_fit_vars["n_Bs_piKpipi"] = new RooRealVar("n_Bs_piKpipi", "", 100, 0, 
            max_yields.at("piKpipi"));
    m_fit_vars["n_low_Kpipipi"] = new RooRealVar("n_low_Kpipipi", "", 100, 0, 
            max_yields.at("Kpipipi"));
    m_fit_vars["n_Bs_low_piKpipi"] = new RooRealVar("n_Bs_low_piKpipi", "", 100, 0, 
            max_yields.at("piKpipi"));
    m_fit_vars["n_rho_Kpipipi"] = new RooRealVar("n_rho_Kpipipi", "", 20, 0, 
            max_yields.at("Kpipipi")/20);
    for (auto mode : m_4_modes) {
        m_fit_vars["n_expo_" + mode] = new RooRealVar(("n_expo_" + mode).c_str(), 
                "", max_yields.at(mode)/2 , 0, max_yields.at(mode));
    }
}


// ==================================================
// Make a PDF to generate toys using data fit results
// ==================================================
RooSimultaneous * ShapeMaker::makeGenerationPdf(std::string results_file) {

    std::cout << "Making toy generation PDF" << std::endl;
    // ========
    // Set seed
    // ========
    RooRandom::randomGenerator()->SetSeed(0);

    // ====================
    // Read in RooFitResult
    // ====================
    std::map<std::string, double> * results = readFitResult(results_file);

    // ====================
    // Set up floating vars
    // ====================
    // Shifts
    m_gen_vars["shift"] = new RooRealVar("toy_shift", "", results->at("shift"));

    // Signal parameters
    m_gen_vars["signal_sigma_L"] = new RooRealVar("toy_signal_sigma_L", "", 
            results->at("signal_sigma_L"));

    // Bs parameters
    m_gen_vars["Bs_sigma_L"] = new RooRealVar("toy_Bs_sigma_L", "", 
            results->at("Bs_sigma_L"));

    // Fraction of 010 shape
    m_gen_vars["low_frac_010_Kpi"] = new RooRealVar("toy_low_frac_010_Kpi", "", 
            results->at("low_frac_010_Kpi"));
    if (m_sum) {
        m_gen_vars["low_frac_010_piK"] = new RooRealVar("toy_low_frac_010_piK", "", 
                results->at("low_frac_010_piK"));
        m_gen_vars["low_frac_010_GLW"] = new RooRealVar("toy_low_frac_010_GLW", "", 
                results->at("low_frac_010_GLW"));
    } else {
        m_gen_vars["low_frac_010_piK_plus"] = new RooRealVar(
                "toy_low_frac_010_piK_plus", "", results->at("low_frac_010_piK_plus"));
        m_gen_vars["low_frac_010_piK_minus"] = new RooRealVar(
                "toy_low_frac_010_piK_minus", "", 
                results->at("low_frac_010_piK_minus"));
        m_gen_vars["low_frac_010_GLW_plus"] = new RooRealVar(
                "toy_low_frac_010_GLW_plus", "", results->at("low_frac_010_GLW_plus"));
        m_gen_vars["low_frac_010_GLW_minus"] = new RooRealVar(
                "toy_low_frac_010_GLW_minus", "", 
                results->at("low_frac_010_GLW_minus"));
    }
    m_gen_vars["Bs_low_frac_010"] = new RooRealVar("toy_Bs_low_frac_010", "", 
            results->at("Bs_low_frac_010"));

    // Slope of exponentials
    m_gen_vars["slope_Kpi"] = new RooRealVar("toy_slope_Kpi", "", 
            results->at("slope_Kpi"));
    m_gen_vars["slope_KK"] = new RooRealVar("toy_slope_KK", "", 
            results->at("slope_KK"));
    m_gen_vars["slope_pipi"] = new RooRealVar("toy_slope_pipi", "", 
            results->at("slope_pipi"));

    // =======================================
    // Set up observables with expected values
    // =======================================
    // Non-blind asymmetries: use value from data fit
    if (!m_sum) {
        m_gen_vars["A_Kpi"] = new RooRealVar("toy_A_Kpi", "", results->at("A_Kpi"));
        m_gen_vars["A_piK_Bs"] = new RooRealVar("toy_A_piK_Bs", "", 
                results->at("A_piK_Bs"));
        m_gen_vars["A_Kpi_low"] = new RooRealVar("toy_A_Kpi_low", "", 
                results->at("A_Kpi_low"));
        m_gen_vars["A_KK_low"] = new RooRealVar("toy_A_KK_low", "", 
                results->at("A_KK_low"));
        m_gen_vars["A_pipi_low"] = new RooRealVar("toy_A_pipi_low", "", 
                results->at("A_pipi_low"));

        // Blind asymmetries: use previous analysis values
        m_gen_vars["A_KK"] = new RooRealVar("toy_A_KK", "", -0.03);
        m_gen_vars["A_pipi"] = new RooRealVar("toy_A_pipi", "", -0.09);
        m_gen_vars["A_KK_Bs"] = new RooRealVar("toy_A_KK_Bs", "", -0.04);
        m_gen_vars["A_pipi_Bs"] = new RooRealVar("toy_A_pipi_Bs", "", 0.06);
    }

    // Non-blind ratios
    m_gen_vars["R_KK_vs_piK_Bs"] = new RooRealVar("toy_R_KK_vs_piK_Bs", "",
            results->at("R_KK_vs_piK_Bs"));
    m_gen_vars["R_pipi_vs_piK_Bs"] = new RooRealVar("toy_R_pipi_vs_piK_Bs", "",
            results->at("R_pipi_vs_piK_Bs"));
    m_gen_vars["R_KK_vs_Kpi_low"] = new RooRealVar("toy_R_KK_vs_Kpi_low", "", 
            results->at("R_KK_vs_Kpi_low"));
    m_gen_vars["R_pipi_vs_Kpi_low"] = new RooRealVar("toy_R_pipi_vs_Kpi_low", "", 
            results->at("R_pipi_vs_Kpi_low"));
    // m_gen_vars["R_KK_vs_piK_Bs_low"] = new RooRealVar("toy_R_KK_vs_piK_Bs_low", "",
            // results->at("R_KK_vs_piK_Bs_low"));
    // m_gen_vars["R_pipi_vs_piK_Bs_low"] = new RooRealVar("toy_R_pipi_vs_piK_Bs_low", "",
            // results->at("R_pipi_vs_piK_Bs_low"));

    // Blind ratios
    m_gen_vars["R_piK_vs_Kpi"] = new RooRealVar("toy_R_piK_vs_Kpi", "", 0.06);
    m_gen_vars["R_KK_vs_Kpi"] = new RooRealVar("toy_R_KK_vs_Kpi", "", 0.11);
    m_gen_vars["R_pipi_vs_Kpi"] = new RooRealVar("toy_R_pipi_vs_Kpi", "", 0.05);
    m_gen_vars["R_plus"] = new RooRealVar("toy_R_plus", "", 0.06);
    m_gen_vars["R_minus"] = new RooRealVar("toy_R_minus", "", 0.06);

    // piK observables used depend on whether fit is summed over flavour
    if (m_sum) { 
        m_gen_vars["R_piK_vs_Kpi_low"] = new RooRealVar("toy_R_piK_vs_Kpi_low", "", 
            results->at("R_piK_vs_Kpi_low"));
    } else {
        m_gen_vars["R_plus_low"] = new RooRealVar("toy_R_plus_low", "", 
                results->at("R_plus_low"));
        m_gen_vars["R_minus_low"] = new RooRealVar("toy_R_minus_low", "", 
                results->at("R_minus_low"));
    }


    // ===========
    // Make yields
    // ===========
    m_gen_vars["n_signal_Kpi"] = new RooRealVar("toy_n_signal_Kpi", "", 
            results->at("n_signal_Kpi"));
    m_gen_vars["n_Bs_piK"] = new RooRealVar("toy_n_Bs_piK", "", 
            results->at("n_Bs_piK"));
    m_gen_vars["n_low_Kpi"] = new RooRealVar("toy_n_low_Kpi", "", 
            results->at("n_low_Kpi"));
    m_gen_vars["n_Bs_low_piK"] = new RooRealVar("toy_n_Bs_low_piK", "", 
            results->at("n_Bs_low_piK"));
    m_gen_vars["n_rho_Kpi"] = new RooRealVar("toy_n_rho_Kpi", "", 
            results->at("n_rho_Kpi"));
    m_gen_vars["n_expo_Kpi"] = new RooRealVar("toy_n_expo_Kpi", "", 
            results->at("n_expo_Kpi"));
    m_gen_vars["n_expo_piK"] = new RooRealVar("toy_n_expo_piK", "", 
            results->at("n_expo_piK"));
    m_gen_vars["n_expo_KK"] = new RooRealVar("toy_n_expo_KK", "", 
            results->at("n_expo_KK"));
    m_gen_vars["n_expo_pipi"] = new RooRealVar("toy_n_expo_pipi", "", 
            results->at("n_expo_pipi"));

    // Make four-body variables if need
    if (m_fourBody) makeGenFourBodyVars(results);

    // Set constants 
    setTwoBodyConstants(m_gen_vars, true);

    // Return simultaneous PDF
    return makePdf(m_gen_vars, m_gen_pdfs, true);
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
RooSimultaneous * ShapeMaker::makePdf(VarMap & vars, PdfMap & pdfs, bool toy_gen,
        bool zero_piK) {

    // Extra string to avoid name duplication
    std::string s = ((toy_gen) ? "toy_" : "");
    if (toy_gen) std::cout << "toy_gen true" << std::endl;
    else std::cout << "toy_gen false" << std::endl;

    // =====================================
    // Set up parameters depending on others
    //======================================
    // Signal shape
    vars["signal_mean"] = new RooFormulaVar((s + "signal_mean").c_str(), "@0 + @1", 
                RooArgList(*vars.at("signal_mean_free"), *vars.at("shift")));
    vars["signal_sigma_R"] = new RooFormulaVar((s + "signal_sigma_R").c_str(), 
            "@0 * @1", RooArgList(*vars.at("signal_sigma_L"), 
                       *vars.at("signal_sigma_ratio")));

    // Bs parameters
    vars["Bs_mean"] = new RooFormulaVar((s + "Bs_mean").c_str(), "@0 + @1", 
            RooArgList(*vars.at("signal_mean"), *vars.at("delta_M")));
    vars["Bs_sigma_R"] = new RooFormulaVar((s + "Bs_sigma_R").c_str(), "@0 * @1", 
            RooArgList(*vars.at("Bs_sigma_L"), 
                       *vars.at("Bs_sigma_ratio")));

    // Rho shape
    vars["rho_mean"] = new RooFormulaVar((s + "rho_mean").c_str(), "@0 + @1", 
            RooArgList(*vars.at("rho_mean_fixed"), *vars.at("shift")));
    vars["rho_sigma_R"] = new RooFormulaVar((s + "rho_sigma_R").c_str(), "@0 * @1", 
            RooArgList(*vars.at("rho_sigma_L"), *vars.at("rho_sigma_ratio")));

    // Bs low mass shape
    vars["Bs_low_a_gamma"] = new RooFormulaVar((s + "Bs_low_a_gamma").c_str(), 
            "@0 + @1", RooArgList(*vars.at("low_a_gamma"), *vars.at("delta_M")));
    vars["Bs_low_b_gamma"] = new RooFormulaVar((s + "Bs_low_b_gamma").c_str(), 
            "@0 + @1", RooArgList(*vars.at("low_b_gamma"), *vars.at("delta_M")));
    vars["Bs_low_a_pi"] = new RooFormulaVar((s + "Bs_low_a_pi").c_str(), "@0 + @1", 
            RooArgList(*vars.at("low_a_pi"), *vars.at("delta_M")));
    vars["Bs_low_b_pi"] = new RooFormulaVar((s + "Bs_low_b_pi").c_str(), "@0 + @1", 
            RooArgList(*vars.at("low_b_pi"), *vars.at("delta_M")));

    // =========
    // Make PDFs
    // =========
    // Signal shape
    pdfs["signal_CB_L"] = new RooCBShape((s + "signal_CB_L").c_str(), "", *m_x, 
            *vars.at("signal_mean"), 
            *vars.at("signal_sigma_L"), 
            *vars.at("signal_alpha_L"), 
            *vars.at("signal_n_L"));
    pdfs["signal_CB_R"] = new RooCBShape((s + "signal_CB_R").c_str(), "", *m_x, 
            *vars.at("signal_mean"), 
            *vars.at("signal_sigma_R"), 
            *vars.at("signal_alpha_R"), 
            *vars.at("signal_n_R"));
    pdfs["signal_shape"] = new RooAddPdf((s + "signal_shape").c_str(), "", 
            RooArgList(*pdfs.at("signal_CB_L"), *pdfs.at("signal_CB_R")), 
            RooArgList(*vars.at("signal_frac")));

    // Bs shape
    pdfs["Bs_CB_L"] = new RooCBShape((s + "Bs_CB_L").c_str(), "", *m_x, 
            *vars.at("Bs_mean"), 
            *vars.at("Bs_sigma_L"), 
            *vars.at("Bs_alpha_L"), 
            *vars.at("Bs_n_L"));
    pdfs["Bs_CB_R"] = new RooCBShape((s + "Bs_CB_R").c_str(), "", *m_x, 
            *vars.at("Bs_mean"), 
            *vars.at("Bs_sigma_R"), 
            *vars.at("Bs_alpha_R"), 
            *vars.at("Bs_n_R"));
    pdfs["Bs_shape"] = new RooAddPdf((s + "Bs_shape").c_str(), "", 
            RooArgList(*pdfs.at("Bs_CB_L"), *pdfs.at("Bs_CB_R")), 
            RooArgList(*vars.at("Bs_frac")));

    // Rho mis-ID shape
    pdfs["rho_CB_L"] = new RooCBShape((s + "rho_CB_L").c_str(), "", *m_x, 
            *vars.at("rho_mean"), 
            *vars.at("rho_sigma_L"), 
            *vars.at("rho_alpha_L"), 
            *vars.at("rho_n_L"));
    pdfs["rho_CB_R"] = new RooCBShape((s + "rho_CB_R").c_str(), "", *m_x, 
            *vars.at("rho_mean"), 
            *vars.at("rho_sigma_R"), 
            *vars.at("rho_alpha_R"), 
            *vars.at("rho_n_R"));
    pdfs["rho_shape"] = new RooAddPdf((s + "rho_shape").c_str(), "", 
            RooArgList(*pdfs.at("rho_CB_L"), *pdfs.at("rho_CB_R")), 
            RooArgList(*vars.at("rho_frac")));

    // Low mass B0 shapes
    pdfs["gamma_010_shape"] = new RooHILLdini((s + "gamma_010_shape").c_str(), "", *m_x, 
            *vars.at("low_a_gamma"), 
            *vars.at("low_b_gamma"), 
            *vars.at("low_csi_gamma_010"), 
            *vars.at("shift"), 
            *vars.at("low_sigma_gamma_010"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));
    pdfs["gamma_101_shape"] = new RooLITTLEHORNSdini((s + "gamma_101_shape").c_str(), "", *m_x, 
            *vars.at("low_a_gamma"), 
            *vars.at("low_b_gamma"), 
            *vars.at("low_csi_gamma_101"), 
            *vars.at("shift"), 
            *vars.at("low_sigma_gamma_101"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"), 
            *vars.at("low_shiftg"));
    pdfs["pi_010_shape"] = new RooHORNSdini((s + "pi_010_shape").c_str(), "", *m_x, 
            *vars.at("low_a_pi"), 
            *vars.at("low_b_pi"), 
            *vars.at("low_csi_pi_010"), 
            *vars.at("shift"), 
            *vars.at("low_sigma_pi_010"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));
    pdfs["pi_101_shape"] = new RooHILLdini((s + "pi_101_shape").c_str(), "", *m_x, 
            *vars.at("low_a_pi"), 
            *vars.at("low_b_pi"), 
            *vars.at("low_csi_pi_101"), 
            *vars.at("shift"), 
            *vars.at("low_sigma_pi_101"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));

    // Low mass Bs shapes
    pdfs["Bs_gamma_010_shape"] = new RooHILLdini((s + "Bs_gamma_010_shape").c_str(), "", *m_x, 
            *vars.at("Bs_low_a_gamma"), 
            *vars.at("Bs_low_b_gamma"), 
            *vars.at("Bs_low_csi_gamma_010"), 
            *vars.at("shift"), 
            *vars.at("Bs_low_sigma_gamma_010"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));
    pdfs["Bs_gamma_101_shape"] = new RooLITTLEHORNSdini((s + "Bs_gamma_101_shape").c_str(), "", 
            *m_x, 
            *vars.at("Bs_low_a_gamma"), 
            *vars.at("Bs_low_b_gamma"), 
            *vars.at("Bs_low_csi_gamma_101"), 
            *vars.at("shift"), 
            *vars.at("Bs_low_sigma_gamma_101"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"), 
            *vars.at("low_shiftg"));
    pdfs["Bs_pi_010_shape"] = new RooHORNSdini((s + "Bs_pi_010_shape").c_str(), "", *m_x, 
            *vars.at("Bs_low_a_pi"), 
            *vars.at("Bs_low_b_pi"), 
            *vars.at("Bs_low_csi_pi_010"), 
            *vars.at("shift"), 
            *vars.at("Bs_low_sigma_pi_010"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));
    pdfs["Bs_pi_101_shape"] = new RooHILLdini((s + "Bs_pi_101_shape").c_str(), "", *m_x, 
            *vars.at("Bs_low_a_pi"), 
            *vars.at("Bs_low_b_pi"), 
            *vars.at("Bs_low_csi_pi_101"), 
            *vars.at("shift"), 
            *vars.at("Bs_low_sigma_pi_101"), 
            *vars.at("low_ratio"), 
            *vars.at("low_frac"));

    // Combine helicity components of low mass shapes
    pdfs["low_010_shape"] = new RooAddPdf((s + "low_010_shape").c_str(), "", 
            RooArgList(*pdfs.at("gamma_010_shape"), *pdfs.at("pi_010_shape")), 
            RooArgList(*vars.at("coeff_gamma_010"), 
                       *vars.at("coeff_pi_010")));
    pdfs["low_101_shape"] = new RooAddPdf((s + "low_101_shape").c_str(), "", 
            RooArgList(*pdfs.at("gamma_101_shape"), *pdfs.at("pi_101_shape")), 
            RooArgList(*vars.at("coeff_gamma_101"), 
                       *vars.at("coeff_pi_101")));
    pdfs["Bs_low_010_shape"] = new RooAddPdf((s + "Bs_low_010_shape").c_str(), "", 
            RooArgList(*pdfs.at("Bs_gamma_010_shape"), *pdfs.at("Bs_pi_010_shape")), 
            RooArgList(*vars.at("coeff_gamma_010"), 
                       *vars.at("coeff_pi_010")));
    pdfs["Bs_low_101_shape"] = new RooAddPdf((s + "Bs_low_101_shape").c_str(), "", 
            RooArgList(*pdfs.at("Bs_gamma_101_shape"), *pdfs.at("Bs_pi_101_shape")), 
            RooArgList(*vars.at("coeff_gamma_101"), 
                       *vars.at("coeff_pi_101")));

    // Combine helicity components
    pdfs["low_Kpi"] = new RooAddPdf((s + "low_Kpi_shape").c_str(), "", 
            RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
            RooArgList(*vars.at("low_frac_010_Kpi")));
    if (m_sum) {
        pdfs["low_piK"] = new RooAddPdf((s + "low_piK_shape").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
                RooArgList(*vars.at("low_frac_010_piK")));
        pdfs["low_KK"] = new RooAddPdf((s + "low_KK_shape").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
                RooArgList(*vars.at("low_frac_010_GLW")));
        pdfs["low_pipi"] = pdfs.at("low_KK");
    }
    pdfs["Bs_low_shape"] = new RooAddPdf((s + "Bs_low_shape").c_str(), "", 
            RooArgList(*pdfs.at("Bs_low_010_shape"), *pdfs.at("Bs_low_101_shape")), 
            RooArgList(*vars.at("Bs_low_frac_010")));

    // Separate shapes for flavour split fit to account for CP violation
    if (!m_sum) {
        pdfs["low_Kpi_plus"] = pdfs.at("low_Kpi");
        pdfs["low_Kpi_minus"] = pdfs.at("low_Kpi");
        pdfs["low_piK_plus"] = new RooAddPdf((s + "low_piK_shape_plus").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
                RooArgList(*vars.at("low_frac_010_piK_plus")));
        pdfs["low_piK_minus"] = new RooAddPdf((s + "low_piK_shape_minus").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
                RooArgList(*vars.at("low_frac_010_piK_minus")));
        pdfs["low_KK_plus"] = new RooAddPdf((s + "low_KK_shape_plus").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")),
                RooArgList(*vars.at("low_frac_010_GLW_plus")));
        pdfs["low_KK_minus"] = new RooAddPdf((s + "low_KK_shape_minus").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")),
                RooArgList(*vars.at("low_frac_010_GLW_minus")));
        pdfs["low_pipi_plus"] = pdfs.at("low_KK_plus");
        pdfs["low_pipi_minus"] = pdfs.at("low_KK_minus");
    }

    // Combinatorial shapes
    pdfs["expo_Kpi"] = new RooExponential((s + "expo_Kpi_shape").c_str(), "", *m_x, 
            *vars.at("slope_Kpi"));
    pdfs["expo_piK"] = pdfs["expo_Kpi"];
    pdfs["expo_KK"] = new RooExponential((s + "expo_KK_shape").c_str(), "", *m_x, 
            *vars.at("slope_KK"));
    pdfs["expo_pipi"] = new RooExponential((s + "expo_pipi_shape").c_str(), "", *m_x, 
            *vars.at("slope_pipi"));

    // ===========
    // Observables
    // ===========
    // Make ratios of minus/plus yields from asymmetries
    // Signal
    if (!m_sum) {
        vars["a_Kpi"] = new RooFormulaVar((s + "a_Kpi").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_Kpi")));
        vars["a_KK"] = new RooFormulaVar((s + "a_KK").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_KK")));
        vars["a_pipi"] = new RooFormulaVar((s + "a_pipi").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_pipi")));

        // Bs 
        vars["a_piK_Bs"] = new RooFormulaVar((s + "a_piK_Bs").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_piK_Bs")));
        vars["a_KK_Bs"] = new RooFormulaVar((s + "a_KK_Bs").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_KK_Bs")));
        vars["a_pipi_Bs"] = new RooFormulaVar((s + "a_pipi_Bs").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_pipi_Bs")));

        // Low mass B0
        vars["a_Kpi_low"] = new RooFormulaVar((s + "a_Kpi_low").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_Kpi_low")));
        vars["a_KK_low"] = new RooFormulaVar((s + "a_KK_low").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_KK_low")));
        vars["a_pipi_low"] = new RooFormulaVar((s + "a_pipi_low").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_pipi_low")));
    }

    // ======
    // Yields
    // ======
    // Total signal yields
    if (m_sum) {
        vars["n_signal_piK"] = new RooFormulaVar((s + "n_signal_piK").c_str(), "@0 * @1", 
                RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_piK_vs_Kpi")));
    }
    vars["n_signal_KK"] = new RooFormulaVar((s + "n_signal_KK").c_str(), "@0 * @1", 
            RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_KK_vs_Kpi")));
    vars["n_signal_pipi"] = new RooFormulaVar((s + "n_signal_pipi").c_str(), "@0 * @1", 
            RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_pipi_vs_Kpi")));

    // Flavour split signal yields
    if (!m_sum) {
        vars["n_signal_Kpi_plus"] = new RooFormulaVar((s + "n_signal_Kpi_plus").c_str(), 
                "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_signal_Kpi"), *vars.at("a_Kpi"))); 
        vars["n_signal_Kpi_minus"] = new RooFormulaVar((s + "n_signal_Kpi_minus").c_str(), 
                "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_signal_Kpi"), *vars.at("a_Kpi"))); 
        vars["n_signal_KK_plus"] = new RooFormulaVar((s + "n_signal_KK_plus").c_str(), 
                "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_signal_KK"), *vars.at("a_KK"))); 
        vars["n_signal_KK_minus"] = new RooFormulaVar((s + "n_signal_KK_minus").c_str(), 
                "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_signal_KK"), *vars.at("a_KK"))); 
        vars["n_signal_pipi_plus"] = new RooFormulaVar((s + "n_signal_pipi_plus").c_str(), 
                "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_signal_pipi"), *vars.at("a_pipi"))); 
        vars["n_signal_pipi_minus"] = new RooFormulaVar((s + "n_signal_pipi_minus").c_str(), 
                "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_signal_pipi"), *vars.at("a_pipi"))); 
    }

    // Make suppressed mode yields from CP ratios
    if (!m_sum) {
        vars["n_signal_piK_plus"] = new RooFormulaVar((s + "n_signal_piK_plus").c_str(), 
                "@0 * @1", 
                RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_plus")));
        vars["n_signal_piK_minus"] = new RooFormulaVar((s + "n_signal_piK_minus").c_str(), "@0 * @1", 
                RooArgList(*vars.at("n_signal_Kpi"), *vars.at("R_minus")));
    }

    // Bs yields
    // vars["n_Bs_KK"] = new RooFormulaVar((s + "n_Bs_KK").c_str(), "@0 * @1",
            // RooArgList(*vars.at("n_Bs_piK"), *vars.at("R_KK_vs_piK_Bs")));
    // vars["n_Bs_pipi"] = new RooFormulaVar((s + "n_Bs_pipi").c_str(), "@0 * @1",
            // RooArgList(*vars.at("n_Bs_piK"), *vars.at("R_pipi_vs_piK_Bs")));
    vars["n_Bs_KK"] = new RooRealVar((s + "n_Bs_KK").c_str(), "", 
            vars.at("R_KK_vs_piK_Bs")->getVal() * 1925.5);
    vars["n_Bs_pipi"] = new RooRealVar((s + "n_Bs_pipi").c_str(), "", 
            vars.at("R_pipi_vs_piK_Bs")->getVal() * 1925.5);
    if (!m_sum) {
        vars["n_Bs_piK_plus"] = new RooFormulaVar((s + "n_Bs_piK_plus").c_str(), 
                "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_Bs_piK"), *vars.at("a_piK_Bs"))); 
        vars["n_Bs_piK_minus"] = new RooFormulaVar((s + "n_Bs_piK_minus").c_str(), 
                "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_Bs_piK"), *vars.at("a_piK_Bs"))); 
        vars["n_Bs_KK_plus"] = new RooFormulaVar((s + "n_Bs_KK_plus").c_str(), 
                "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_Bs_KK"), *vars.at("a_KK_Bs"))); 
        vars["n_Bs_KK_minus"] = new RooFormulaVar((s + "n_Bs_KK_minus").c_str(), 
                "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_Bs_KK"), *vars.at("a_KK_Bs"))); 
        vars["n_Bs_pipi_plus"] = new RooFormulaVar((s + "n_Bs_pipi_plus").c_str(), 
                "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_Bs_pipi"), *vars.at("a_pipi_Bs"))); 
        vars["n_Bs_pipi_minus"] = new RooFormulaVar((s + "n_Bs_pipi_minus").c_str(), 
                "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_Bs_pipi"), *vars.at("a_pipi_Bs"))); 
    }

    // Low mass yields
    if (m_sum) {
        vars["n_low_piK"] = new RooFormulaVar((s + "n_low_piK").c_str(), "@0 * @1", 
                RooArgList(*vars.at("n_low_Kpi"), *vars.at("R_piK_vs_Kpi_low")));
    }
    vars["n_low_KK"] = new RooFormulaVar((s + "n_low_KK").c_str(), "@0 * @1", 
            RooArgList(*vars.at("n_low_Kpi"), *vars.at("R_KK_vs_Kpi_low")));
    vars["n_low_pipi"] = new RooFormulaVar((s + "n_low_pipi").c_str(), "@0 * @1", 
            RooArgList(*vars.at("n_low_Kpi"), *vars.at("R_pipi_vs_Kpi_low")));

    // Flavour split low mass yields
    if (!m_sum) {
        vars["n_low_Kpi_plus"] = new RooFormulaVar((s + "n_low_Kpi_plus").c_str(), "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_low_Kpi"), *vars.at("a_Kpi_low")));
        vars["n_low_Kpi_minus"] = new RooFormulaVar((s + "n_low_Kpi_minus").c_str(), 
                "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_low_Kpi"), *vars.at("a_Kpi_low")));
        vars["n_low_KK_plus"] = new RooFormulaVar((s + "n_low_KK_plus").c_str(), "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_low_KK"), *vars.at("a_KK_low")));
        vars["n_low_KK_minus"] = new RooFormulaVar((s + "n_low_KK_minus").c_str(), 
                "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_low_KK"), *vars.at("a_KK_low")));
        vars["n_low_pipi_plus"] = new RooFormulaVar((s + "n_low_pipi_plus").c_str(), 
                "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_low_pipi"), *vars.at("a_pipi_low")));
        vars["n_low_pipi_minus"] = new RooFormulaVar((s + "n_low_pipi_minus").c_str(), 
                "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_low_pipi"), *vars.at("a_pipi_low")));
    }

    // Split piK low mass yields using ratios
    if (!m_sum) {
        vars["n_low_piK_plus"] = new RooFormulaVar((s + "n_low_piK_plus").c_str(), "@0 * @1", 
                RooArgList(*vars.at("n_low_Kpi_plus"), *vars.at("R_plus_low")));
        vars["n_low_piK_minus"] = new RooFormulaVar((s + "n_low_piK_minus").c_str(), 
                "@0 * @1", 
                RooArgList(*vars.at("n_low_Kpi_minus"), *vars.at("R_minus_low")));
    }

    // Bs low mass yields
    vars["n_Bs_low_KK"] = new RooFormulaVar((s + "n_Bs_low_KK").c_str(), "@0 * @1", 
            RooArgList(*vars.at("n_Bs_low_piK"), 
                       *vars.at("R_KK_vs_piK_Bs_low")));
    vars["n_Bs_low_pipi"] = new RooFormulaVar((s + "n_Bs_low_pipi").c_str(), "@0 * @1", 
            RooArgList(*vars.at("n_Bs_low_piK"), 
                       *vars.at("R_pipi_vs_piK_Bs_low")));

    // Flavour split Bs low mass yields (no asymmetry, shared equally)
    vars["n_Bs_low_piK_plus"] = new RooFormulaVar((s + "n_Bs_low_piK_plus").c_str(), 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_piK")));
    vars["n_Bs_low_piK_minus"] = new RooFormulaVar((s + "n_Bs_low_piK_minus").c_str(), 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_piK")));
    vars["n_Bs_low_KK_plus"] = new RooFormulaVar((s + "n_Bs_low_KK_plus").c_str(), 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_KK")));
    vars["n_Bs_low_KK_minus"] = new RooFormulaVar((s + "n_Bs_low_KK_minus").c_str(), 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_KK")));
    vars["n_Bs_low_pipi_plus"] = new RooFormulaVar((s + "n_Bs_low_pipi_plus").c_str(), 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_pipi")));
    vars["n_Bs_low_pipi_minus"] = new RooFormulaVar((s + "n_Bs_low_pipi_minus").c_str(), 
            "@0 / 2", RooArgList(*vars.at("n_Bs_low_pipi")));

    // Rho yields
    vars["n_rho_piK"] = vars["n_rho_Kpi"];
    vars["n_rho_KK"] = new RooFormulaVar((s + "n_rho_KK").c_str(), "@0 * @1", 
            RooArgList(*vars.at("n_rho_piK"), *vars.at("R_KK_vs_piK_Bs_low")));
    vars["n_rho_pipi"] = new RooFormulaVar((s + "n_rho_pipi").c_str(), "@0 * @1", 
            RooArgList(*vars.at("n_rho_piK"), *vars.at("R_pipi_vs_piK_Bs_low")));

    // Flavour split rho yields (split equally)
    vars["n_rho_Kpi_plus"] = new RooFormulaVar((s + "n_rho_Kpi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_Kpi")));
    vars["n_rho_Kpi_minus"] = new RooFormulaVar((s + "n_rho_Kpi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_Kpi")));
    vars["n_rho_piK_plus"] = new RooFormulaVar((s + "n_rho_piK_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_piK")));
    vars["n_rho_piK_minus"] = new RooFormulaVar((s + "n_rho_piK_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_piK")));
    vars["n_rho_KK_plus"] = new RooFormulaVar((s + "n_rho_KK_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_KK")));
    vars["n_rho_KK_minus"] = new RooFormulaVar((s + "n_rho_KK_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_KK")));
    vars["n_rho_pipi_plus"] = new RooFormulaVar((s + "n_rho_pipi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_pipi")));
    vars["n_rho_pipi_minus"] = new RooFormulaVar((s + "n_rho_pipi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_pipi")));

    // Flavour split exponential yields (split equally)
    vars["n_expo_Kpi_plus"] = new RooFormulaVar((s + "n_expo_Kpi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_Kpi")));
    vars["n_expo_Kpi_minus"] = new RooFormulaVar((s + "n_expo_Kpi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_Kpi")));
    vars["n_expo_piK_plus"] = new RooFormulaVar((s + "n_expo_piK_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_piK")));
    vars["n_expo_piK_minus"] = new RooFormulaVar((s + "n_expo_piK_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_piK")));
    vars["n_expo_KK_plus"] = new RooFormulaVar((s + "n_expo_KK_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_KK")));
    vars["n_expo_KK_minus"] = new RooFormulaVar((s + "n_expo_KK_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_KK")));
    vars["n_expo_pipi_plus"] = new RooFormulaVar((s + "n_expo_pipi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_pipi")));
    vars["n_expo_pipi_minus"] = new RooFormulaVar((s + "n_expo_pipi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_pipi")));


    // Make each two-body PDF
    makeTwoBodyPdfs(vars, pdfs, toy_gen, zero_piK);

    // Also make four-body if required
    if (m_fourBody) makeFourBodyPdfs(vars, pdfs, toy_gen);

    // Return simultaneous PDF
    return makeSimPdf(pdfs, toy_gen);

}


// ====================================
// Make four-body fixed yields and PDFs
// ====================================
void ShapeMaker::makeFourBodyPdfs(VarMap & vars, PdfMap & pdfs, bool toy_gen) {

    std::cout << "Making four body PDFS" << std::endl;
    // Extra string to avoid name duplication
    std::string s = ((toy_gen) ? "toy_" : "");

    // =======================
    // Set up fixed parameters
    // =======================
    // Signal parameters
    vars["4_signal_mean_free"] = new RooRealVar((s + "4_signal_mean_free").c_str(), 
            "", m_pr->getParam("4_signal_mean")); 
    vars["4_signal_mean"] = new RooFormulaVar((s + "4_signal_mean").c_str(), 
            "@0 + @1", RooArgList(*vars.at("signal_mean_free"), *vars.at("shift"))); 
    vars["4_signal_sigma_ratio"] = new 
        RooRealVar((s + "4_signal_sigma_ratio").c_str(), "", 
            m_pr->getParam("4_signal_sigma_ratio"));
    vars["4_signal_sigma_R"] = new RooFormulaVar((s + "4_signal_sigma_R").c_str(), 
            "@0 * @1", RooArgList(*vars.at("4_signal_sigma_L"), 
                *vars.at("4_signal_sigma_ratio")));
    vars["4_signal_alpha_L"] = new RooRealVar((s + "4_signal_alpha_L").c_str(), "", 
            m_pr->getParam("4_signal_alpha_L"));
    vars["4_signal_alpha_R"] = new RooRealVar((s + "4_signal_alpha_R").c_str(), "", 
            m_pr->getParam("4_signal_alpha_R"));
    vars["4_signal_n_L"] = new RooRealVar((s + "4_signal_n_L").c_str(), "", 
            m_pr->getParam("4_signal_n_L"));
    vars["4_signal_n_R"] = new RooRealVar((s + "4_signal_n_R").c_str(), "", 
            m_pr->getParam("4_signal_n_R"));
    vars["4_signal_frac"] = new RooRealVar((s + "4_signal_frac").c_str(), "", 
            m_pr->getParam("4_signal_frac"));

    // Bs parameters
    vars["4_Bs_mean"] = new RooFormulaVar((s + "4_Bs_mean").c_str(), "@0 + @1", 
            RooArgList(*vars.at("4_signal_mean"), *vars.at("delta_M")));
    vars["4_Bs_sigma_ratio"] = new RooRealVar((s + "4_Bs_sigma_ratio").c_str(), "", 
            (*m_pr)["Bs_sigma_ratio"]);
    vars["4_Bs_sigma_R"] = new RooFormulaVar((s + "4_Bs_sigma_R").c_str(), 
            "@0 * @1", RooArgList(*vars.at("4_Bs_sigma_L"), 
                       *vars.at("4_Bs_sigma_ratio")));
    vars["4_Bs_alpha_L"] = new RooRealVar((s + "4_Bs_alpha_L").c_str(), "", 
            m_pr->getParam("Bs_alpha_L"));
    vars["4_Bs_alpha_R"] = new RooRealVar((s + "4_Bs_alpha_R").c_str(), "", 
            m_pr->getParam("Bs_alpha_R"));
    vars["4_Bs_n_L"] = new RooRealVar((s + "4_Bs_n_L").c_str(), "", 
            m_pr->getParam("Bs_n_L"));
    vars["4_Bs_n_R"] = new RooRealVar((s + "4_Bs_n_R").c_str(), "", 
            m_pr->getParam("Bs_n_R"));
    vars["4_Bs_frac"] = new RooRealVar((s + "4_Bs_frac").c_str(), "", 
            m_pr->getParam("Bs_frac"));

    // =========
    // Make PDFs
    // =========
    // Signal shape
    pdfs["4_signal_CB_L"] = new RooCBShape((s + "4_signal_CB_L").c_str(), "", *m_x, 
            *vars.at("4_signal_mean"), 
            *vars.at("4_signal_sigma_L"), 
            *vars.at("4_signal_alpha_L"), 
            *vars.at("4_signal_n_L"));
    pdfs["4_signal_CB_R"] = new RooCBShape((s + "4_signal_CB_R").c_str(), "", *m_x, 
            *vars.at("4_signal_mean"), 
            *vars.at("4_signal_sigma_R"), 
            *vars.at("4_signal_alpha_R"), 
            *vars.at("4_signal_n_R"));
    pdfs["4_signal_shape"] = new RooAddPdf((s + "4_signal_shape").c_str(), "", 
            RooArgList(*pdfs.at("4_signal_CB_L"), *pdfs.at("4_signal_CB_R")), 
            RooArgList(*vars.at("4_signal_frac")));

    // Bs shape
    pdfs["4_Bs_CB_L"] = new RooCBShape((s + "4_Bs_CB_L").c_str(), "", *m_x, 
            *vars.at("4_Bs_mean"), 
            *vars.at("4_Bs_sigma_L"), 
            *vars.at("4_Bs_alpha_L"), 
            *vars.at("4_Bs_n_L"));
    pdfs["4_Bs_CB_R"] = new RooCBShape((s + "4_Bs_CB_R").c_str(), "", *m_x, 
            *vars.at("4_Bs_mean"), 
            *vars.at("4_Bs_sigma_R"), 
            *vars.at("4_Bs_alpha_R"), 
            *vars.at("4_Bs_n_R"));
    pdfs["4_Bs_shape"] = new RooAddPdf((s + "4_Bs_shape").c_str(), "", 
            RooArgList(*pdfs.at("4_Bs_CB_L"), *pdfs.at("4_Bs_CB_R")), 
            RooArgList(*vars.at("4_Bs_frac")));

    // Combine helicity components of low mass shapes
    pdfs["low_Kpipipi"] = new RooAddPdf((s + "low_Kpipipi_shape").c_str(), "", 
            RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
            RooArgList(*vars.at("4_low_frac_010_Kpi")));
    if (m_sum) {
        pdfs["low_piKpipi"] = new RooAddPdf((s + "low_piKpipi_shape").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
                RooArgList(*vars.at("4_low_frac_010_piK")));
        pdfs["low_pipipipi"] = new RooAddPdf((s + "low_pipipipi_shape").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
                RooArgList(*vars.at("4_low_frac_010_GLW")));
    }

    // Separate shapes for flavour split fit to account for CP violation
    if (!m_sum) {
        pdfs["low_Kpipipi_plus"] = pdfs.at("low_Kpipipi");
        pdfs["low_Kpipipi_minus"] = pdfs.at("low_Kpipipi");
        pdfs["low_piKpipi_plus"] = new 
            RooAddPdf((s + "low_piKpipi_shape_plus").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
                RooArgList(*vars.at("4_low_frac_010_piK_plus")));
        pdfs["low_piKpipi_minus"] = new 
            RooAddPdf((s + "low_piKpipi_shape_minus").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")), 
                RooArgList(*vars.at("4_low_frac_010_piK_minus")));
        pdfs["low_pipipipi_plus"] = new 
            RooAddPdf((s + "low_pipipipi_shape_plus").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")),
                RooArgList(*vars.at("4_low_frac_010_GLW_plus")));
        pdfs["low_pipipipi_minus"] = new 
            RooAddPdf((s + "low_pipipipi_shape_minus").c_str(), "", 
                RooArgList(*pdfs.at("low_010_shape"), *pdfs.at("low_101_shape")),
                RooArgList(*vars.at("low_frac_010_GLW_minus")));
    }

    // Combinatorial shapes
    pdfs["expo_Kpipipi"] = new 
        RooExponential((s + "expo_Kpipipi_shape").c_str(), "", *m_x, 
            *vars.at("slope_Kpipipi"));
    pdfs["expo_piKpipi"] = pdfs["expo_Kpipipi"];
    pdfs["expo_pipipipi"] = new 
        RooExponential((s + "expo_pipipipi_shape").c_str(), "", *m_x, 
            *vars.at("slope_pipipipi"));

    // ===========
    // Observables
    // ===========
    // Make ratios of minus/plus yields from asymmetries
    // Signal
    if (!m_sum) {
        vars["a_Kpipipi"] = new 
            RooFormulaVar((s + "a_Kpipipi").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_Kpipipi")));
        vars["a_pipipipi"] = new 
            RooFormulaVar((s + "a_pipipipi").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_pipipipi")));

        // Bs 
        vars["a_piKpipi_Bs"] = new 
            RooFormulaVar((s + "a_piKpipi_Bs").c_str(), "(1 + @0) / (1 - @0)", 
                RooArgList(*vars.at("A_piKpipi_Bs")));
        vars["a_pipipipi_Bs"] = new RooFormulaVar((s + "a_pipipipi_Bs").c_str(), 
                "(1 + @0) / (1 - @0)", RooArgList(*vars.at("A_pipipipi_Bs")));

        // Low mass B0
        vars["a_Kpipipi_low"] = new RooFormulaVar((s + "a_Kpipipi_low").c_str(), 
                "(1 + @0) / (1 - @0)", RooArgList(*vars.at("A_Kpipipi_low")));
        vars["a_pipipipi_low"] = new RooFormulaVar((s + "a_pipipipi_low").c_str(), 
                "(1 + @0) / (1 - @0)", RooArgList(*vars.at("A_pipipipi_low")));
    }

    // ======
    // Yields
    // ======
    // Total signal yields
    if (m_sum) {
        vars["n_signal_piKpipi"] = new 
            RooFormulaVar((s + "n_signal_piKpipi").c_str(), "@0 * @1", 
                RooArgList(*vars.at("n_signal_Kpipipi"), 
                    *vars.at("R_piKpipi_vs_Kpipipi")));
    }
    vars["n_signal_pipipipi"] = new RooFormulaVar((s + "n_signal_pipipipi").c_str(), 
            "@0 * @1", RooArgList(*vars.at("n_signal_Kpipipi"), 
                *vars.at("R_pipipipi_vs_Kpipipi")));

    // Flavour split signal yields
    if (!m_sum) {
        vars["n_signal_Kpipipi_plus"] = new 
            RooFormulaVar((s + "n_signal_Kpipipipipi_plus").c_str(), 
                "@0 / (1 + @1)", RooArgList(*vars.at("n_signal_Kpipipipipi"), 
                    *vars.at("a_Kpipipipipi"))); 
        vars["n_signal_Kpipipi_minus"] = new 
            RooFormulaVar((s + "n_signal_Kpipipi_minus").c_str(), "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_signal_Kpipipi"), *vars.at("a_Kpipipi"))); 
        vars["n_signal_pipipipi_plus"] = new 
            RooFormulaVar((s + "n_signal_pipipipi_plus").c_str(), "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_signal_pipipipi"), *vars.at("a_pipipipi"))); 
        vars["n_signal_pipipipi_minus"] = new 
            RooFormulaVar((s + "n_signal_pipipipi_minus").c_str(), 
                "@0 / (1 + 1/@1)", RooArgList(*vars.at("n_signal_pipipipi"), 
                    *vars.at("a_pipipipi"))); 
    }

    // Make suppressed mode yields from CP ratios
    if (!m_sum) {
        vars["n_signal_piKpipi_plus"] = new 
            RooFormulaVar((s + "n_signal_piKpipi_plus").c_str(), "@0 * @1", 
                RooArgList(*vars.at("n_signal_Kpipipi"), *vars.at("4_R_plus")));
        vars["n_signal_piKpipi_minus"] = new 
            RooFormulaVar((s + "n_signal_piKpipi_minus").c_str(), "@0 * @1", 
                RooArgList(*vars.at("n_signal_Kpipipi"), *vars.at("4_R_minus")));
    }

    // Bs yields
    vars["n_Bs_pipipipi"] = new RooFormulaVar((s + "n_Bs_pipipipi").c_str(), 
            "@0 * @1", RooArgList(*vars.at("n_Bs_piKpipi"), 
                *vars.at("R_pipipipi_vs_piKpipi_Bs")));
    if (!m_sum) {
        vars["n_Bs_piKpipi_plus"] = new 
            RooFormulaVar((s + "n_Bs_piKpipi_plus").c_str(), "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_Bs_piKpipi"), *vars.at("a_piKpipi_Bs"))); 
        vars["n_Bs_piKpipi_minus"] = new 
            RooFormulaVar((s + "n_Bs_piKpipi_minus").c_str(), "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_Bs_piKpipi"), *vars.at("a_piKpipi_Bs"))); 
        vars["n_Bs_pipipipi_plus"] = new 
            RooFormulaVar((s + "n_Bs_pipipipi_plus").c_str(), "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_Bs_pipipipi"), *vars.at("a_pipipipi_Bs"))); 
        vars["n_Bs_pipipipi_minus"] = new 
            RooFormulaVar((s + "n_Bs_pipipipi_minus").c_str(), "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_Bs_pipipipi"), *vars.at("a_pipipipi_Bs"))); 
    }

    // Low mass yields
    if (m_sum) {
        vars["n_low_piKpipi"] = new RooFormulaVar((s + "n_low_piKpipi").c_str(), 
                "@0 * @1", RooArgList(*vars.at("n_low_Kpipipi"), 
                    *vars.at("R_piKpipi_vs_Kpipipi_low")));
    }
    vars["n_low_pipipipi"] = new RooFormulaVar((s + "n_low_pipipipi").c_str(), 
            "@0 * @1", RooArgList(*vars.at("n_low_Kpipipi"), 
                *vars.at("R_pipipipi_vs_Kpipipi_low")));

    // Flavour split low mass yields
    if (!m_sum) {
        vars["n_low_Kpipipi_plus"] = new 
            RooFormulaVar((s + "n_low_Kpipipi_plus").c_str(), "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_low_Kpipipi"), *vars.at("a_Kpipipi_low")));
        vars["n_low_Kpipipi_minus"] = new 
            RooFormulaVar((s + "n_low_Kpipipi_minus").c_str(), "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_low_Kpipipi"), *vars.at("a_Kpipipi_low")));
        vars["n_low_pipipipi_plus"] = new 
            RooFormulaVar((s + "n_low_pipipipi_plus").c_str(), "@0 / (1 + @1)", 
                RooArgList(*vars.at("n_low_pipipipi"), *vars.at("a_pipipipi_low")));
        vars["n_low_pipipipi_minus"] = new 
            RooFormulaVar((s + "n_low_pipipipi_minus").c_str(), "@0 / (1 + 1/@1)", 
                RooArgList(*vars.at("n_low_pipipipi"), *vars.at("a_pipipipi_low")));
    }

    // Split piK low mass yields using ratios
    if (!m_sum) {
        vars["n_low_piKpipi_plus"] = new 
            RooFormulaVar((s + "n_low_piKpipi_plus").c_str(), "@0 * @1", 
                RooArgList(*vars.at("n_low_Kpipipi_plus"), 
                    *vars.at("4_R_plus_low")));
        vars["n_low_piKpipi_minus"] = new 
            RooFormulaVar((s + "n_low_piKpipi_minus").c_str(), "@0 * @1", 
                RooArgList(*vars.at("n_low_Kpipipi_minus"), 
                    *vars.at("4_R_minus_low")));
    }

    // Bs low mass yields
    vars["R_pipipipi_vs_piKpipi_Bs_low"] = new 
        RooRealVar((s + "R_pipipipi_vs_piKpipi_Bs_low").c_str(), "", 0.0327);
    vars["n_Bs_low_pipipipi"] = new RooFormulaVar((s + "n_Bs_low_pipipipi").c_str(), 
            "@0 * @1", RooArgList(*vars.at("n_Bs_low_piKpipi"), 
                       *vars.at("R_pipipipi_vs_piKpipi_Bs_low")));

    // Flavour split Bs low mass yields (no asymmetry, shared equally)
    vars["n_Bs_low_piKpipi_plus"] = new 
        RooFormulaVar((s + "n_Bs_low_piKpipi_plus").c_str(), "@0 / 2", 
                RooArgList(*vars.at("n_Bs_low_piKpipi")));
    vars["n_Bs_low_piKpipi_minus"] = new 
        RooFormulaVar((s + "n_Bs_low_piKpipi_minus").c_str(), "@0 / 2", 
                RooArgList(*vars.at("n_Bs_low_piKpipi")));
    vars["n_Bs_low_pipipipi_plus"] = new 
        RooFormulaVar((s + "n_Bs_low_pipipipi_plus").c_str(), "@0 / 2", 
                RooArgList(*vars.at("n_Bs_low_pipipipi")));
    vars["n_Bs_low_pipipipi_minus"] = new 
        RooFormulaVar((s + "n_Bs_low_pipipipi_minus").c_str(), "@0 / 2", 
                RooArgList(*vars.at("n_Bs_low_pipipipi")));

    // Rho yields
    vars["n_rho_piKpipi"] = vars["n_rho_Kpipipi"];
    vars["n_rho_pipipipi"] = new RooFormulaVar((s + "n_rho_pipipipi").c_str(), 
            "@0 * @1", RooArgList(*vars.at("n_rho_piKpipi"), 
                *vars.at("R_pipipipi_vs_piKpipi_Bs_low")));

    // Flavour split rho yields (split equally)
    vars["n_rho_Kpipipi_plus"] = new 
        RooFormulaVar((s + "n_rho_Kpipipi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_Kpipipi")));
    vars["n_rho_Kpipipi_minus"] = new 
        RooFormulaVar((s + "n_rho_Kpipipi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_Kpipipi")));
    vars["n_rho_piKpipi_plus"] = new 
        RooFormulaVar((s + "n_rho_piKpipi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_piKpipi")));
    vars["n_rho_piKpipi_minus"] = new 
        RooFormulaVar((s + "n_rho_piKpipi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_piKpipi")));
    vars["n_rho_pipipipi_plus"] = new 
        RooFormulaVar((s + "n_rho_pipipipi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_pipipipi")));
    vars["n_rho_pipipipi_minus"] = new 
        RooFormulaVar((s + "n_rho_pipipipi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_rho_pipipipi")));

    // Flavour split exponential yields (split equally)
    vars["n_expo_Kpipipi_plus"] = new 
        RooFormulaVar((s + "n_expo_Kpipipi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_Kpipipi")));
    vars["n_expo_Kpipipi_minus"] = new 
        RooFormulaVar((s + "n_expo_Kpipipi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_Kpipipi")));
    vars["n_expo_piKpipi_plus"] = new 
        RooFormulaVar((s + "n_expo_piKpipi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_piKpipi")));
    vars["n_expo_piKpipi_minus"] = new 
        RooFormulaVar((s + "n_expo_piKpipi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_piKpipi")));
    vars["n_expo_pipipipi_plus"] = new 
        RooFormulaVar((s + "n_expo_pipipipi_plus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_pipipipi")));
    vars["n_expo_pipipipi_minus"] = new 
        RooFormulaVar((s + "n_expo_pipipipi_minus").c_str(), "@0 / 2", 
            RooArgList(*vars.at("n_expo_pipipipi")));

    // ===================
    // Make each fit shape
    // ===================
    std::cout << "Looping over 4-body modes to make fit PDFs" << std::endl;
    for (auto mode : m_4_modes) {
        if (m_sum) {

            // Lists of shapes and yields
            RooArgList shapes_list;
            RooArgList yields_list;

            // Add shapes
            shapes_list.add(*pdfs.at("4_signal_shape"));
            shapes_list.add(*pdfs.at("expo_" + mode));
            shapes_list.add(*pdfs.at("low_" + mode));
            shapes_list.add(*pdfs.at("rho_shape"));
            if (mode != "Kpipipi") {
                shapes_list.add(*pdfs.at("4_Bs_shape"));
                shapes_list.add(*pdfs.at("Bs_low_shape"));
            }

            // Add yields
            yields_list.add(*vars.at("n_signal_" + mode));
            yields_list.add(*vars.at("n_expo_" + mode));
            yields_list.add(*vars.at("n_low_" + mode));
            yields_list.add(*vars.at("n_rho_" + mode));
            if (mode != "Kpipipi") {
                yields_list.add(*vars.at("n_Bs_" + mode));
                yields_list.add(*vars.at("n_Bs_low_" + mode));
            }

            // Make shape
            pdfs["fit_" + mode] = new RooAddPdf((mode + "_fit").c_str(), "", 
                    shapes_list, yields_list);

            // Save shapes to check toy generation works ok
            if (toy_gen) {
                TCanvas * canv = new TCanvas("canvas", "", 500, 400);
                RooPlot * frame = m_x->frame();
                pdfs.at("fit_" + mode)->plotOn(frame);
                frame->Draw();
                canv->SaveAs(("../Plots/toy_" + mode + ".pdf").c_str());
                delete canv;

                // Sum yields and put in map if generating a toy
                TIterator * it = yields_list.createIterator();
                int n_tot = 0;
                RooRealVar * yield;
                while ((yield = (RooRealVar*)it->Next())) {
                    n_tot += yield->getVal();
                }
                m_expectedYields[mode] = n_tot;
                m_yieldsCalculated = true;
            }

        } else {

            // List of shapes and yields
            std::cout << "Making fit shape for " << mode << std::endl;
            RooArgList shapes_list_plus;
            RooArgList shapes_list_minus;
            RooArgList yields_list_plus;
            RooArgList yields_list_minus;

            // Add shapes
            shapes_list_plus.add(*pdfs.at("4_signal_shape"));
            shapes_list_minus.add(*pdfs.at("4_signal_shape"));
            shapes_list_plus.add(*pdfs.at("expo_" + mode));
            shapes_list_minus.add(*pdfs.at("expo_" + mode));
            shapes_list_plus.add(*pdfs.at("low_" + mode + "_plus"));
            shapes_list_minus.add(*pdfs.at("low_" + mode + "_minus"));
            shapes_list_plus.add(*pdfs.at("rho_shape"));
            shapes_list_minus.add(*pdfs.at("rho_shape"));
            if (mode != "Kpipipi") {
                shapes_list_plus.add(*pdfs.at("4_Bs_shape"));
                shapes_list_minus.add(*pdfs.at("4_Bs_shape"));
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
            if (mode != "Kpipipi") {
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

            // Save shapes to check toy generation works ok
            if (toy_gen) {
                TCanvas * canv = new TCanvas("toy", "", 500, 400);
                RooPlot * frame = m_x->frame();
                pdfs.at("fit_" + mode + "_plus")->plotOn(frame);
                frame->Draw();
                canv->SaveAs(("../Plots/toy_" + mode + "_plus.pdf").c_str());
                canv->Clear();
                RooPlot * frame2 = m_x->frame();
                pdfs.at("fit_" + mode + "_minus")->plotOn(frame2);
                frame2->Draw();
                canv->SaveAs(("../Plots/toy_" + mode + "_minus.pdf").c_str());
                delete canv;
            }

            // Sum the yields in the list and fill map, if generating a toy
            if (toy_gen) {
                TIterator * it_plus = yields_list_plus.createIterator();
                int n_tot_plus = 0;
                RooRealVar * yield;
                while ((yield = (RooRealVar*)it_plus->Next())) {
                    n_tot_plus += yield->getVal();
                }
                m_expectedYields[mode + "_plus"] = n_tot_plus; 
                TIterator * it_minus = yields_list_minus.createIterator();
                int n_tot_minus = 0;
                while ((yield = (RooRealVar*)it_minus->Next())) {
                    n_tot_minus += yield->getVal();
                }
                m_expectedYields[mode + "_minus"] = n_tot_minus;
                m_expectedYields[mode] = n_tot_plus + n_tot_minus;
                m_yieldsCalculated = true;
            }
        } // End if statement about m_sum
    } // End loop over modes
}



// =============================================
// Function to save PDFs as histograms in a file
// =============================================
void ShapeMaker::saveFitHistograms(std::string filename, 
        std::map<std::string, RooDataSet*> dataMap, bool blind) {

    // Open the file
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");

    // Loop through modes
    std::vector<std::string> modes = m_modes;
    if (m_fourBody) {
        modes.push_back("Kpipipi");
        modes.push_back("piKpipi");
        modes.push_back("pipipipi");
    }
    for (auto mode : modes) {
        std::cout << "Saving " << mode << std::endl;

        // Categories to use for each mode
        std::vector<std::string> mode_categories;
        if (m_sum) {
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
            TH1F * h_signal;
            if (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") {
                h_signal= (TH1F*)m_fit_pdfs.at("4_signal_shape")->createHistogram(
                    ("h_signal_" + fullname).c_str(), *m_x, 
                    RooFit::Binning(nBins * 10));
            } else {
                h_signal= (TH1F*)m_fit_pdfs.at("signal_shape")->createHistogram(
                    ("h_signal_" + fullname).c_str(), *m_x, 
                    RooFit::Binning(nBins * 10));
            }
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
            h_signal->Scale(m_fit_vars.at("n_signal_" + fullname)->getVal() * 10 / 
                    h_signal->Integral());
            h_expo->Scale(m_fit_vars.at("n_expo_" + fullname)->getVal() * 10 / 
                    h_expo->Integral());
            h_low->Scale(m_fit_vars.at("n_low_" + fullname)->getVal() * 10 / 
                    h_low->Integral());
            h_rho->Scale(m_fit_vars.at("n_rho_" + fullname)->getVal() * 10 / 
                    h_rho->Integral());

            // Save unblinded histos for Kpi
            if (mode == "Kpi" || mode == "Kpipipi") {

                // Scale fit histgram
                h_fit->Scale((h_signal->Integral() + h_expo->Integral() + 
                        h_low->Integral() + h_rho->Integral())/h_fit->Integral());

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
                TH1F * h_Bs;
                if (mode == "piKpipi" || mode == "pipipipi") {
                    h_Bs = (TH1F*)m_fit_pdfs.at("4_Bs_shape")->createHistogram(
                        ("h_Bs_" + fullname).c_str(), *m_x, 
                        RooFit::Binning(nBins * 10));
                } else {
                    h_Bs = (TH1F*)m_fit_pdfs.at("Bs_shape")->createHistogram(
                        ("h_Bs_" + fullname).c_str(), *m_x, 
                        RooFit::Binning(nBins * 10));
                }
                TH1F * h_Bs_low = (TH1F*)m_fit_pdfs.at("Bs_low_shape")->createHistogram(
                        ("h_Bs_low_" + fullname).c_str(), *m_x, 
                        RooFit::Binning(nBins * 10));

                // Scale Bs histograms
                h_Bs->Scale(m_fit_vars.at("n_Bs_" + fullname)->getVal() * 10 / 
                        h_Bs->Integral());
                h_Bs_low->Scale(m_fit_vars.at("n_Bs_low_" + fullname)->getVal() * 10 / 
                        h_Bs_low->Integral());

                // Scale fit histogram
                h_fit->Scale((h_signal->Integral() + h_expo->Integral() + 
                        h_low->Integral() + h_rho->Integral() + 
                        h_Bs->Integral() + h_Bs_low->Integral())/h_fit->Integral());

                // Make histograms which can be blinded
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

                // Remove data from blind region if blinding required
                if (blind) {
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
                }

                // Save histograms to file
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


// ===================================================
// Return yields for each category from toy generation
// ===================================================
YieldMap ShapeMaker::getExpectedYields() {

    // Check if the map has been filled
    if (!m_yieldsCalculated) {
        std::cout << "Error: yield map has not been filled! " <<
            "ShapeMaker::makeGenerationPdf() needs to be called first."
            << std::endl;
        exit(EXIT_FAILURE);
    }
    return m_expectedYields;
}


// =====================
// Access a fit variable
// =====================
RooAbsReal * ShapeMaker::getFitVariable(std::string name) {
    return m_fit_vars.at(name);
}


// ===========================================
// Make PDF with yields from previous analysis
// ===========================================
RooSimultaneous * ShapeMaker::makePreviousAnalysisPdf(FixedPdfOpt opt) {
    if (opt == FixedPdfOpt::Default) {
        return makePreviousAnalysisPdf(m_gen_vars, opt);
    } else {
        return makePreviousAnalysisPdf(m_fit_vars, opt);
    }
}

RooSimultaneous * ShapeMaker::makePreviousAnalysisPdf(VarMap & vars,
       FixedPdfOpt opt) {

    // ======================
    // Prefix for RooRealVars
    // ======================
    std::string s = (opt == FixedPdfOpt::Default) ? "toy_" : "";
    std::cout << "Using prefix " << s << std::endl;
    gRandom->SetSeed(0);

    // ====================
    // Read in RooFitResult
    // ====================
    std::map<std::string, double> * results = readFitResult("../Results/"
            "twoBody_2011:2012:2015:2016_combined_binned.root");

    // ====================
    // Set up floating vars
    // ====================
    // Shifts
    vars["shift"] = new RooRealVar((s + "shift").c_str(), "", results->at("shift"));

    // Signal parameters
    vars["signal_sigma_L"] = new RooRealVar((s + "signal_sigma_L").c_str(), "", 
            results->at("signal_sigma_L"));

    // Bs parameters
    vars["Bs_sigma_L"] = new RooRealVar((s + "Bs_sigma_L").c_str(), "", 
            results->at("Bs_sigma_L"));

    // Fraction of 010 shape
    vars["low_frac_010_Kpi"] = new RooRealVar((s + "low_frac_010_Kpi").c_str(), "", 
            results->at("low_frac_010_Kpi"));
    vars["Bs_low_frac_010"] = new RooRealVar((s + "Bs_low_frac_010").c_str(), "", 
            results->at("Bs_low_frac_010"));
    if (m_sum) {
        vars["low_frac_010_piK"] = new RooRealVar((s + "low_frac_010_piK").c_str(), "", 
                results->at("low_frac_010_piK"));
        vars["low_frac_010_GLW"] = new RooRealVar((s + "low_frac_010_GLW").c_str(), "", 
                results->at("low_frac_010_GLW"));
    } else {
        vars["low_frac_010_piK_plus"] = new RooRealVar(
                (s + "low_frac_010_piK_plus").c_str(), "", results->at("low_frac_010_piK_plus"));
        vars["low_frac_010_piK_minus"] = new RooRealVar(
                (s + "low_frac_010_piK_minus").c_str(), "", 
                results->at("low_frac_010_piK_minus"));
        vars["low_frac_010_GLW_plus"] = new RooRealVar(
                (s + "low_frac_010_GLW_plus").c_str(), "", results->at("low_frac_010_GLW_plus"));
        vars["low_frac_010_GLW_minus"] = new RooRealVar(
                (s + "low_frac_010_GLW_minus").c_str(), "", 
                results->at("low_frac_010_GLW_minus"));
    }

    // // Fix Bs helicity fraction to value from previous analysis
    // vars["Bs_low_frac_010"] = new RooRealVar((s + "Bs_low_frac_010").c_str(), "",
            // 0.7);

    // Slope of exponentials
    vars["slope_Kpi"] = new RooRealVar((s + "slope_Kpi").c_str(), "", 
            results->at("slope_Kpi"));
    vars["slope_KK"] = new RooRealVar((s + "slope_KK").c_str(), "", 
            results->at("slope_KK"));
    vars["slope_pipi"] = new RooRealVar((s + "slope_pipi").c_str(), "", 
            results->at("slope_pipi"));

    // =====================================================
    // Set up observables with values from previous analysis
    // =====================================================

    // Non-blind ratios
    vars["R_KK_vs_piK_Bs"] = new RooRealVar((s + "R_KK_vs_piK_Bs").c_str(), "",
            239.2/1925.5);
    vars["R_pipi_vs_piK_Bs"] = new RooRealVar((s + "R_pipi_vs_piK_Bs").c_str(), "",
            73.5/1925.5);
    vars["R_KK_vs_Kpi_low"] = new RooRealVar((s + "R_KK_vs_Kpi_low").c_str(), "", 
            111.4/1044.0);
    vars["R_pipi_vs_Kpi_low"] = new RooRealVar((s + "R_pipi_vs_Kpi_low").c_str(), "", 
            52.7/1044.0);
    vars["R_KK_vs_piK_Bs_low"] = new RooRealVar((s + "R_KK_vs_piK_Bs_low").c_str(), 
            "", 0.103);
    vars["R_pipi_vs_piK_Bs_low"] = new RooRealVar((s + "R_pipi_vs_piK_Bs_low").c_str(), 
            "", 0.0337);

    // R_ADS depends on option
    if (opt == FixedPdfOpt::Default) {
        vars["R_piK_vs_Kpi"] = new RooRealVar((s + "R_piK_vs_Kpi").c_str(), 
                "", 50.1/774.9);
    } else if (opt == FixedPdfOpt::FreeRatio) {
        vars["R_piK_vs_Kpi"] = new RooRealVar((s + "R_piK_vs_Kpi").c_str(), 
                "", 0.06, 0, 0.3);
    } else if (opt == FixedPdfOpt::ZeroRatio) {
        vars["R_piK_vs_Kpi"] = new RooRealVar((s + "R_piK_vs_Kpi").c_str(), 
                "", 0);
    }

    // Blind ratios
    vars["R_KK_vs_Kpi"] = new RooRealVar((s + "R_KK_vs_Kpi").c_str(), "", 88.9/774.9);
    vars["R_pipi_vs_Kpi"] = new RooRealVar((s + "R_pipi_vs_Kpi").c_str(), "", 38.7/774.9);
    vars["R_plus"] = new RooRealVar((s + "R_plus").c_str(), "", 0.06);
    vars["R_minus"] = new RooRealVar((s + "R_minus").c_str(), "", 0.06);

    // piK observables used depend on whether fit is summed over flavour
    if (m_sum) { 
        vars["R_piK_vs_Kpi_low"] = new RooRealVar((s + "R_piK_vs_Kpi_low").c_str(), "", 
            376.5/2015.0);
    } else {
        vars["R_plus_low"] = new RooRealVar((s + "R_plus_low").c_str(), "", 
                results->at("R_plus_low"));
        vars["R_minus_low"] = new RooRealVar((s + "R_minus_low").c_str(), "", 
                results->at("R_minus_low"));
    }


    // ===========
    // Make yields
    // ===========
    vars["n_signal_Kpi"] = new RooRealVar((s + "n_signal_Kpi").c_str(), "", 774.9);
    vars["n_low_Kpi"] = new RooRealVar((s + "n_low_Kpi").c_str(), "", 1044.0);
    vars["n_Bs_low_piK"] = new RooRealVar((s + "n_Bs_low_piK").c_str(), "", 2051.6);
    vars["n_rho_Kpi"] = new RooRealVar((s + "n_rho_Kpi").c_str(), "", 
            results->at("n_rho_Kpi") * 774.9 / results->at("n_signal_Kpi"));
    vars["n_expo_Kpi"] = new RooRealVar((s + "n_expo_Kpi").c_str(), "", 1768.9);
    vars["n_expo_piK"] = new RooRealVar((s + "n_expo_piK").c_str(), "", 922.6);
    vars["n_expo_KK"] = new RooRealVar((s + "n_expo_KK").c_str(), "", 556.8);
    vars["n_expo_pipi"] = new RooRealVar((s + "n_expo_pipi").c_str(), "", 201.6);

    // Free Bs yield if making a fit PDF
    if (opt != FixedPdfOpt::Default) {
        vars["n_Bs_piK"] = new RooRealVar((s + "n_Bs_piK").c_str(), "", 100, 0, 8000);
    } else {
        vars["n_Bs_piK"] = new RooRealVar((s + "n_Bs_piK").c_str(), "", 1925.5);
    }

    // Return simultaneous PDF
    if (opt == FixedPdfOpt::Default) {
        setTwoBodyConstants(vars, true);
        return makePdf(vars, m_gen_pdfs, true);
    } else {
        setTwoBodyConstants(m_fit_vars, false);
        if (opt == FixedPdfOpt::FreeRatio) {
            return makePdf(m_fit_vars, m_fit_pdfs, false);
        } else {
            return makePdf(m_fit_vars, m_fit_pdfs, false, true);
        }
    }
}


// ================================================
// Make four-body variables based on a RooFitResult
// ================================================
void ShapeMaker::makeGenFourBodyVars(std::map<std::string, double> * results) {

    std::cout << "Making four-body variables for toy generation PDF" << std::endl;
    // Signal parameters
    m_gen_vars["4_signal_sigma_L"] = new RooRealVar("toy_4_signal_sigma_L", "",
            results->at("4_signal_sigma_L"));

    // Bs parameters
    m_gen_vars["4_Bs_sigma_L"] = new RooRealVar("toy_4_Bs_sigma_L", "", 
            results->at("4_Bs_sigma_L"));

    // Floating fraction of 010 shape
    if (m_sum) {
        m_gen_vars["4_low_frac_010_Kpi"] = new RooRealVar("toy_4_low_frac_010_Kpi", 
                "", results->at("4_low_frac_010_Kpi"));
        m_gen_vars["4_low_frac_010_piK"] = new RooRealVar("toy_4_low_frac_010_piK", 
            "", results->at("4_low_frac_010_piK"));
        m_gen_vars["4_low_frac_010_GLW"] = new RooRealVar("toy_4_low_frac_010_GLW", 
            "", results->at("4_low_frac_010_GLW"));
    } else {
        m_gen_vars["4_low_frac_010_piK_plus"] = new 
            RooRealVar("toy_4_low_frac_010_piK_plus", "", 
                    results->at("4_low_frac_010_piK_plus"));
        m_gen_vars["4_low_frac_010_piK_minus"] = new 
            RooRealVar("toy_4_low_frac_010_piK_minus", "", 
                   results->at("4_low_frac_010_piK_minus"));
        m_gen_vars["4_low_frac_010_GLW_plus"] = new 
            RooRealVar("toy_4_low_frac_010_GLW_plus", "", 
                    results->at("4_low_frac_010_GLW_plus"));
        m_gen_vars["4_low_frac_010_GLW_minus"] = new RooRealVar(
                "toy_4_low_frac_010_GLW_minus", "", 
                results->at("4_low_frac_010_GLW_minus"));
    }

    // Slope of exponentials
    m_gen_vars["slope_Kpipipi"] = new RooRealVar("toy_slope_Kpipipi", "", 
            results->at("slope_Kpipipi"));
    m_gen_vars["slope_pipipipi"] = new RooRealVar("toy_slope_pipipipi", "", 
            results->at("slope_pipipipi"));

    // ===========================
    // Set up floating observables
    // ===========================
    if (!m_sum) {
        // Non-blind asymmetries: use fit result
        m_gen_vars["A_Kpipipi"] = new RooRealVar("toy_A_Kpipipi", "", 
                results->at("A_Kpipipi"));
        m_gen_vars["A_piKpipi_Bs"] = new RooRealVar("toy_A_piKpipi_Bs", "", 
                results->at("A_piKpipi_Bs"));
        m_gen_vars["A_pipipipi_Bs"] = new RooRealVar("toy_A_pipipipi_Bs", "", 
                results->at("A_pipipipi_Bs"));
        m_gen_vars["A_Kpipipi_low"] = new RooRealVar("toy_A_Kpipipi_low", "", 
                results->at("A_Kpipipi_low"));
        m_gen_vars["A_pipipipi_low"] = new RooRealVar("toy_A_pipipipi_low", "", 
                results->at("A_pipipipi_low"));

        // Blind: use two-body previous analysis results
        m_gen_vars["A_pipipipi"] = new RooRealVar("toy_A_pipipipi", "", -0.09);
    }

    // Signal ratios
    // Suppressed to favoured ADS ratio
    if (m_sum) {
        m_gen_vars["R_piKpipi_vs_Kpipipi"] = new 
            RooRealVar("toy_R_piKpipi_vs_Kpipipi", "", 0.06);
        m_gen_vars["R_piKpipi_vs_Kpipipi_low"] = new 
            RooRealVar("toy_R_piKpipi_vs_Kpipipi_low", "", 
                    results->at("R_piKpipi_vs_Kpipipi_low"));
    } else {
        m_gen_vars["4_R_plus"] = new RooRealVar("toy_4_R_plus", "", 0.06);
        m_gen_vars["4_R_minus"] = new RooRealVar("toy_4_R_minus", "",  0.06);
        m_gen_vars["4_R_plus_low"] = new RooRealVar("toy_4_R_plus_low", "", 
                results->at("4_R_plus_low"));
        m_gen_vars["4_R_minus_low"] = new RooRealVar("toy_4_R_minus_low", "", 
                results->at("4_R_minus_low"));
    }

    // CP modes to favoured ADS ratio
    m_gen_vars["R_pipipipi_vs_Kpipipi"] = new 
        RooRealVar("toy_R_pipipipi_vs_Kpipipi", "", 0.05);

    // Bs ratios
    m_gen_vars["R_pipipipi_vs_piKpipi_Bs"] = new 
        RooRealVar("toy_R_pipipipi_vs_piKpipi_Bs", "", 
                results->at("R_pipipipi_vs_piKpipi_Bs"));

    // Low mass ratios
    m_gen_vars["R_pipipipi_vs_Kpipipi_low"] = new 
        RooRealVar("toy_R_pipipipi_vs_Kpipipi_low", "", 
                results->at("R_pipipipi_vs_Kpipipi_low"));

    // ====================
    // Make floating yields
    // ====================
    m_gen_vars["n_signal_Kpipipi"] = new RooRealVar("toy_n_signal_Kpipipi", "", 
            results->at("n_signal_Kpipipi"));
    m_gen_vars["n_Bs_piKpipi"] = new RooRealVar("toy_n_Bs_piKpipi", "", 
            results->at("n_Bs_piKpipi"));
    m_gen_vars["n_low_Kpipipi"] = new RooRealVar("toy_n_low_Kpipipi", "", 
            results->at("n_low_Kpipipi"));
    m_gen_vars["n_Bs_low_piKpipi"] = new RooRealVar("toy_n_Bs_low_piKpipi", "", 
            results->at("n_Bs_low_piKpipi"));
    m_gen_vars["n_rho_Kpipipi"] = new RooRealVar("toy_n_rho_Kpipipi", "", 
            results->at("n_rho_Kpipipi"));
    for (auto mode : m_4_modes) {
        m_gen_vars["n_expo_" + mode] = new 
            RooRealVar(("toy_n_expo_" + mode).c_str(), "", 
                    results->at("n_expo_" + mode));
    }
}


// =============================================
// Shift a fixed variable within its uncertainty
// =============================================
void ShapeMaker::shiftVariable(std::string name, double sigma) {
    
    // Check variable is constant
    if (!m_fit_vars.at(name)->isConstant()) {
        std::cout << "Warning: smearing a non-constant variable " << name << 
            std::endl;
    }

    // Adjust value around mean
    double mean = m_fit_vars.at(name)->getVal();
    gRandom->SetSeed(0);
    double new_val = gRandom->Gaus(mean, sigma);
    m_fit_vars[name] = new RooRealVar(name.c_str(), "", new_val);
    std::cout << "Shifted value for " << name << " from " << mean << " to " <<
        new_val << std::endl;
}


// ============================
// Make each two body fit shape
// ============================
void ShapeMaker::makeTwoBodyPdfs(VarMap & vars, PdfMap & pdfs, bool toy_gen,
        bool zero_piK) {

    TCanvas * canv = new TCanvas("canvas", "", 500, 400);
    for (auto mode : m_modes) {
        if (m_sum) {
            // Lists of shapes and yields
            RooArgList shapes_list;
            RooArgList yields_list;

            // Add shapes
            if (!(mode == "piK" && zero_piK)) {
                shapes_list.add(*pdfs.at("signal_shape"));
            }
            shapes_list.add(*pdfs.at("expo_" + mode));
            shapes_list.add(*pdfs.at("low_" + mode));
            shapes_list.add(*pdfs.at("rho_shape"));
            if (mode != "Kpi") {
                shapes_list.add(*pdfs.at("Bs_shape"));
                shapes_list.add(*pdfs.at("Bs_low_shape"));
            }

            // Add yields
            if (!(mode == "piK" && zero_piK)) {
                yields_list.add(*vars.at("n_signal_" + mode));
            }
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

            // Save shapes to check toy generation works ok
            if (toy_gen) {
                RooPlot * frame = m_x->frame();
                pdfs.at("fit_" + mode)->plotOn(frame);
                frame->Draw();
                canv->SaveAs(("../Plots/toy_" + mode + ".pdf").c_str());
                canv->Clear();

                // Sum yields and put in map if generating a toy
                TIterator * it = yields_list.createIterator();
                int n_tot = 0;
                RooRealVar * yield;
                while ((yield = (RooRealVar*)it->Next())) {
                    n_tot += yield->getVal();
                }
                m_expectedYields[mode] = n_tot;
                m_yieldsCalculated = true;
            }

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

            // Save shapes to check toy generation works ok
            if (toy_gen) {
                RooPlot * frame = m_x->frame();
                pdfs.at("fit_" + mode + "_plus")->plotOn(frame);
                frame->Draw();
                canv->SaveAs(("../Plots/toy_" + mode + "_plus.pdf").c_str());
                canv->Clear();
                RooPlot * frame2 = m_x->frame();
                pdfs.at("fit_" + mode + "_minus")->plotOn(frame2);
                frame2->Draw();
                canv->SaveAs(("../Plots/toy_" + mode + "_minus.pdf").c_str());
                canv->Clear();
            }

            // Sum the yields in the list and fill map, if generating a toy
            if (toy_gen) {
                TIterator * it_plus = yields_list_plus.createIterator();
                int n_tot_plus = 0;
                RooRealVar * yield;
                while ((yield = (RooRealVar*)it_plus->Next())) {
                    n_tot_plus += yield->getVal();
                }
                m_expectedYields[mode + "_plus"] = n_tot_plus; 
                TIterator * it_minus = yields_list_minus.createIterator();
                int n_tot_minus = 0;
                while ((yield = (RooRealVar*)it_minus->Next())) {
                    n_tot_minus += yield->getVal();
                }
                m_expectedYields[mode + "_minus"] = n_tot_minus;
                m_expectedYields[mode] = n_tot_plus + n_tot_minus;
                m_yieldsCalculated = true;
            }
        }
    }
}

// ===========================================
// Combine PDFs to make final simultaneous PDF
// ===========================================
RooSimultaneous * ShapeMaker::makeSimPdf(PdfMap & pdfs, bool toy_gen) {

    // Extra string to avoid name duplication
    std::string s = ((toy_gen) ? "toy_" : "");

    RooSimultaneous * simPdf = new RooSimultaneous((s + "simPdf").c_str(), "", 
            *m_cat);
    for (auto mode : m_modes) {
        if (m_sum) {
            simPdf->addPdf(*pdfs.at("fit_" + mode), mode.c_str());
        } else {
            simPdf->addPdf(*pdfs.at("fit_" + mode + "_plus"), 
                    (mode + "_plus").c_str());
            simPdf->addPdf(*pdfs.at("fit_" + mode + "_minus"), 
                    (mode + "_minus").c_str());
        }
    }

    // Add four-body modes if using
    if (m_fourBody) {
        for (auto mode : m_4_modes) {
            if (m_sum) {
                simPdf->addPdf(*pdfs.at("fit_" + mode), mode.c_str());
            } else {
                simPdf->addPdf(*pdfs.at("fit_" + mode + "_plus"), 
                        (mode + "_plus").c_str());
                simPdf->addPdf(*pdfs.at("fit_" + mode + "_minus"), 
                        (mode + "_minus").c_str());
            }
        }
    }

    simPdf->Print("v");

    // Print value of R_KK_vs_piK_Bs_low
    m_fit_vars["R_KK_vs_piK_Bs_low"]->Print();

    return simPdf;
}


// ===================================================
// Set two-body constant parameters for fit generation
// ===================================================
void ShapeMaker::setTwoBodyConstants(VarMap & vars, bool toy_gen) {

    // Extra string to avoid name duplication
    std::string s = ((toy_gen) ? "toy_" : "");

    // Signal parameters
    vars["signal_mean_free"] = new RooRealVar((s + "signal_mean_free").c_str(), "", 
            m_pr->getParam("signal_mean")); 
    vars["signal_sigma_ratio"] = new RooRealVar((s + "signal_sigma_ratio").c_str(), 
            "", m_pr->getParam("signal_sigma_ratio"));
    vars["signal_alpha_L"] = new RooRealVar((s + "signal_alpha_L").c_str(), "", 
            m_pr->getParam("signal_alpha_L"));
    vars["signal_alpha_R"] = new RooRealVar((s + "signal_alpha_R").c_str(), "", 
            m_pr->getParam("signal_alpha_R"));
    vars["signal_n_L"] = new RooRealVar((s + "signal_n_L").c_str(), "", 
            m_pr->getParam("signal_n_L"));
    vars["signal_n_R"] = new RooRealVar((s + "signal_n_R").c_str(), "", 
            m_pr->getParam("signal_n_R"));
    vars["signal_frac"] = new RooRealVar((s + "signal_frac").c_str(), "", 
            m_pr->getParam("signal_frac"));

    // Bs parameters
    vars["delta_M"] = new RooRealVar((s + "delta_M").c_str(), "", 87.26);
    vars["Bs_sigma_ratio"] = new RooRealVar((s + "Bs_sigma_ratio").c_str(), "", 
            (*m_pr)["Bs_sigma_ratio"]);
    vars["Bs_alpha_L"] = new RooRealVar((s + "Bs_alpha_L").c_str(), "", 
            m_pr->getParam("Bs_alpha_L"));
    vars["Bs_alpha_R"] = new RooRealVar((s + "Bs_alpha_R").c_str(), "", 
            m_pr->getParam("Bs_alpha_R"));
    vars["Bs_n_L"] = new RooRealVar((s + "Bs_n_L").c_str(), "", 
            m_pr->getParam("Bs_n_L"));
    vars["Bs_n_R"] = new RooRealVar((s + "Bs_n_R").c_str(), "", 
            m_pr->getParam("Bs_n_R"));
    vars["Bs_frac"] = new RooRealVar((s + "Bs_frac").c_str(), "", 
            m_pr->getParam("Bs_frac"));

    // Rho mis-ID shape
    vars["rho_mean_fixed"] = new RooRealVar((s + "rho_mean_fixed").c_str(), "", 
            m_pr->getParam("rho_mean"));
    vars["rho_sigma_L"] = new RooRealVar((s + "rho_sigma_L").c_str(), "", 
            m_pr->getParam("rho_sigma_L"));
    vars["rho_sigma_ratio"] = new RooRealVar((s + "rho_sigma_ratio").c_str(), "", 
            m_pr->getParam("rho_sigma_ratio"));
    vars["rho_alpha_L"] = new RooRealVar((s + "rho_alpha_L").c_str(), "", 
            m_pr->getParam("rho_alpha_L"));
    vars["rho_alpha_R"] = new RooRealVar((s + "rho_alpha_R").c_str(), "", 
            m_pr->getParam("rho_alpha_R"));
    vars["rho_n_L"] = new RooRealVar((s + "rho_n_L").c_str(), "", 
            m_pr->getParam("rho_n_L"));
    vars["rho_n_R"] = new RooRealVar((s + "rho_n_R").c_str(), "", 
            m_pr->getParam("rho_n_R"));
    vars["rho_frac"] = new RooRealVar((s + "rho_frac").c_str(), "", 
            m_pr->getParam("rho_frac"));

    // Low mass B0 parameters
    vars["low_a_gamma"] = new RooRealVar((s + "low_a_gamma").c_str(), "", 
            m_pr->getParam("low_a_gamma"));
    vars["low_b_gamma"] = new RooRealVar((s + "low_b_gamma").c_str(), "", 
            m_pr->getParam("low_b_gamma"));
    vars["low_a_pi"] = new RooRealVar((s + "low_a_pi").c_str(), "", 
            m_pr->getParam("low_a_pi"));
    vars["low_b_pi"] = new RooRealVar((s + "low_b_pi").c_str(), "", 
            m_pr->getParam("low_b_pi"));
    vars["low_sigma_gamma_010"] = new RooRealVar((s + 
                "low_sigma_gamma_010").c_str(), "", 
            m_pr->getParam("low_sigma_gamma_010"));
    vars["low_sigma_gamma_101"] = new RooRealVar((s + 
                "low_sigma_gamma_101").c_str(), "", 
            m_pr->getParam("low_sigma_gamma_101"));
    vars["low_sigma_pi_010"] = new RooRealVar((s + 
                "low_sigma_pi_010").c_str(), "", 
            m_pr->getParam("low_sigma_pi_010"));
    vars["low_sigma_pi_101"] = new RooRealVar((s + "low_sigma_pi_101").c_str(), "", 
            m_pr->getParam("low_sigma_pi_101"));
    vars["low_csi_gamma_010"] = new RooRealVar((s + "low_csi_gamma_010").c_str(), 
            "", m_pr->getParam("low_csi_gamma_010"));
    vars["low_csi_gamma_101"] = new RooRealVar((s + "low_csi_gamma_101").c_str(), 
            "", m_pr->getParam("low_csi_gamma_101"));
    vars["low_csi_pi_010"] = new RooRealVar((s + "low_csi_pi_010").c_str(), "", 
            m_pr->getParam("low_csi_pi_010"));
    vars["low_csi_pi_101"] = new RooRealVar((s + "low_csi_pi_101").c_str(), "", 
            m_pr->getParam("low_csi_pi_101"));
    vars["low_frac"] = new RooRealVar((s + "low_frac").c_str(), "", 
            m_pr->getParam("low_frac"));
    vars["low_ratio"] = new RooRealVar((s + "low_ratio").c_str(), "", 
            m_pr->getParam("low_ratio"));
    vars["low_shiftg"] = new RooRealVar((s + "low_shiftg").c_str(), "", 0);

    // Low mass Bs parameters
    vars["Bs_low_sigma_gamma_010"] = new RooRealVar((s + 
                "Bs_low_sigma_gamma_010").c_str(), "", 
            m_pr->getParam("low_Bs_sigma_gamma_010"));
    vars["Bs_low_sigma_gamma_101"] = new RooRealVar(
            (s + "Bs_low_sigma_gamma_101").c_str(), "", 
            m_pr->getParam("low_Bs_sigma_gamma_101"));
    vars["Bs_low_sigma_pi_010"] = new RooRealVar((s + 
                "Bs_low_sigma_pi_010").c_str(), "", 
            m_pr->getParam("low_Bs_sigma_pi_010"));
    vars["Bs_low_sigma_pi_101"] = new RooRealVar((s + 
                "Bs_low_sigma_pi_101").c_str(), "", 
            m_pr->getParam("low_Bs_sigma_pi_101"));
    vars["Bs_low_csi_gamma_010"] = new RooRealVar((s + 
                "Bs_low_csi_gamma_010").c_str(), 
            "", m_pr->getParam("low_Bs_csi_gamma_010"));
    vars["Bs_low_csi_gamma_101"] = new RooRealVar((s + 
                "Bs_low_csi_gamma_101").c_str(), 
            "", m_pr->getParam("low_Bs_csi_gamma_101"));
    vars["Bs_low_csi_pi_010"] = new RooRealVar((s + "Bs_low_csi_pi_010").c_str(), 
            "", m_pr->getParam("low_Bs_csi_pi_010"));
    vars["Bs_low_csi_pi_101"] = new RooRealVar((s + "Bs_low_csi_pi_101").c_str(), 
            "", m_pr->getParam("low_Bs_csi_pi_101"));

    // Parameters for combining low mass shapes
    const double branching_gamma = 0.353;
    const double branching_pi = 0.647;
    const double acc_gamma = 17.48;
    const double acc_pi = 15.53;
    double G_010 = branching_gamma * acc_gamma * 0.009227;
    double G_101 = branching_gamma * acc_gamma * 0.004613; 
    double P_010 = branching_pi * acc_pi * 0.01168;
    double P_101 = branching_pi * acc_pi * 0.005712;
    vars["coeff_gamma_010"] = new RooRealVar((s + "coeff_gamma_010").c_str(), "", 
            G_010 / (P_010 + G_010));
    vars["coeff_gamma_101"] = new RooRealVar((s + "coeff_gamma_101").c_str(), "", 
            G_101 / (P_101 + G_101));
    vars["coeff_pi_010"] = new RooRealVar((s + "coeff_pi_010").c_str(), "", 
            P_010 / (P_010 + G_010));
    vars["coeff_pi_101"] = new RooRealVar((s + "coeff_pi_101").c_str(), "", 
            P_101 / (P_101 + G_101));

    // Fixed yield ratios
    vars["R_KK_vs_piK_Bs_low"] = new RooRealVar((s + "R_KK_vs_piK_Bs_low").c_str(),
            "", 0.103);
    vars["R_pipi_vs_piK_Bs_low"] = new RooRealVar((s + 
                "R_pipi_vs_piK_Bs_low").c_str(), "", 0.0337);

}
