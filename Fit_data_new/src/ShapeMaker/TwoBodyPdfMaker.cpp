#include "TwoBodyPdfMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
TwoBodyPdfMaker::TwoBodyPdfMaker(RooRealVar * x, RooCategory * cat, bool blind) :
    TwoBodyBase("pdf", x, cat),
    m_blind(blind) {}

TwoBodyPdfMaker::TwoBodyPdfMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, bool blind) : 
    TwoBodyBase(name, x, cat),
    m_blind(blind) {}


// ==========
// Destructor
// ==========
TwoBodyPdfMaker::~TwoBodyPdfMaker() {}


// ===============
// Save histograms
// ===============
void TwoBodyPdfMaker::SaveHistograms(std::string filename) {
    if (m_blind) {
        std::cout << "Error: blindness is turned on! Can't save histograms" 
            << std::endl;
    } else {
        ShapeMakerBase::SaveHistograms(filename);
    }
}


// =========================
// Save histograms with data
// =========================
void TwoBodyPdfMaker::SaveHistograms(std::string filename, RooDataHist * data) {
    if (m_blind) {
        std::cout << "Error: blindness is turned on! Can't save histograms" 
            << std::endl;
    } else {
        ShapeMakerBase::SaveHistograms(filename, data);
    }
}

// =======================
// Set floating parameters
// =======================
void TwoBodyPdfMaker::SetFloatingParameters() {

    // Global shift
    m_pars->AddRealVar("shift", 0, -10, 10);

    // Signal widths
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Fit_monte_carlo/Results/");
    pr->ReadParameters("signal", "signal_Kpi.param");
    pr->ReadParameters("Bs", "signal_Bs.param");
    m_pars->AddRealVar("signal_sigma_L", pr->GetValue("signal", "sigma_L"),
            pr->GetValue("signal", "sigma_L") - 10, 
            pr->GetValue("signal", "sigma_L") + 10);
    m_pars->AddRealVar("Bs_sigma_L", pr->GetValue("Bs", "sigma_L"),
            pr->GetValue("Bs", "sigma_L") - 10, 
            pr->GetValue("Bs", "sigma_L") + 10);

    // Helicity fraction for low mass shapes
    m_pars->AddRealVar("low_frac_010_Kpi", 0.5, 0, 1);
    m_pars->AddRealVar("low_frac_010_piK", 0.5, 0, 1);
    m_pars->AddRealVar("low_frac_010_GLW", 0.5, 0, 1);
    m_pars->AddRealVar("Bs_low_frac_010", 0.5, 0, 1);

    // Slope of exponentials
    m_pars->AddRealVar("slope_Kpi", -0.005, -0.5, 0);
    m_pars->AddRealVar("slope_KK", -0.005, -0.5, 0);
    m_pars->AddRealVar("slope_pipi", -0.005, -0.5, 0);

    // Signal yield ratios
    std::string extra_string = m_blind ? "_blind" : "";
    m_pars->AddRealVar("R_piK_vs_Kpi" + extra_string, 0.06, 0, 3);
    m_pars->AddRealVar("R_KK_vs_Kpi" + extra_string, 0.3, 0, 1);
    m_pars->AddRealVar("R_pipi_vs_Kpi" + extra_string, 0.1, 0, 1);
    if (m_blind) {
        m_pars->AddUnblindVar("R_piK_vs_Kpi", "R_piK_vs_Kpi_blind", 
                "blind_piK_ratio", 0.01);
        m_pars->AddUnblindVar("R_KK_vs_Kpi", "R_KK_vs_Kpi_blind", 
                "blind_KK_ratio", 0.1);
        m_pars->AddUnblindVar("R_pipi_vs_Kpi", "R_pipi_vs_Kpi_blind", 
                "blind_pipi_ratio", 0.05);
    }

    // Bs ratios
    m_pars->AddRealVar("R_KK_vs_piK_Bs", 0.3, 0, 1);
    m_pars->AddRealVar("R_pipi_vs_piK_Bs", 0.1, 0, 1);

    // Low mass ratios
    m_pars->AddRealVar("R_piK_vs_Kpi_low", 0.06, 0, 1);
    m_pars->AddRealVar("R_KK_vs_Kpi_low", 0.3, 0, 1);
    m_pars->AddRealVar("R_pipi_vs_Kpi_low", 0.1, 0, 1);

    // Floating yields
    m_pars->AddRealVar("n_signal_Kpi", 100, 0, 8000);
    m_pars->AddRealVar("n_Bs_piK", 100, 0, 8000);
    m_pars->AddRealVar("n_low_Kpi", 100, 0, 8000);
    m_pars->AddRealVar("n_Bs_low_piK", 100, 0, 8000);
    m_pars->AddRealVar("n_rho_Kpi", 20, 0, 200);
    m_pars->AddRealVar("n_DKpipi_Kpi", 20, 0, 500);

    // Exponential yields
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi"};
    for (auto mode : modes) {
        m_pars->AddRealVar("n_expo_" + mode, 100, 0, 8000);
    }
}
