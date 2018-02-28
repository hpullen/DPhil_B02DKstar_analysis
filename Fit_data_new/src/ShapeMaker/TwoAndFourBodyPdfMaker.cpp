#include "TwoAndFourBodyPdfMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
TwoAndFourBodyPdfMaker::TwoAndFourBodyPdfMaker(RooRealVar * x, RooCategory * cat, bool blind) :
    TwoAndFourBodyBase("pdf", x, cat),
    m_blind(blind) {}

TwoAndFourBodyPdfMaker::TwoAndFourBodyPdfMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, bool blind) : 
    TwoAndFourBodyBase(name, x, cat),
    m_blind(blind) {}


// ==========
// Destructor
// ==========
TwoAndFourBodyPdfMaker::~TwoAndFourBodyPdfMaker() {}


// ===============
// Save histograms
// ===============
void TwoAndFourBodyPdfMaker::SaveHistograms(std::string filename) {
    ShapeMakerBase::SaveHistograms(filename, m_blind);
}


// =========================
// Save histograms with data
// =========================
void TwoAndFourBodyPdfMaker::SaveHistograms(std::string filename, RooDataHist * data) {
    ShapeMakerBase::SaveHistograms(filename, data, m_blind);
}


// =======================
// Set floating parameters
// =======================
void TwoAndFourBodyPdfMaker::SetFloatingParameters() {

    // Two body floating parameters
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
    m_pars->AddRealVar("n_signal_Kpi", 100, 0, GetMaxYield("Kpi"));
    m_pars->AddRealVar("n_Bs_piK", 100, 0, GetMaxYield("piK"));
    m_pars->AddRealVar("n_low_Kpi", 100, 0, GetMaxYield("Kpi"));
    m_pars->AddRealVar("n_Bs_low_piK", 100, 0, GetMaxYield("piK"));
    m_pars->AddRealVar("n_rho_Kpi", 20, 0, GetMaxYield("Kpi")/20);
    m_pars->AddRealVar("n_DKpipi_Kpi", 20, 0, GetMaxYield("Kpi")/5);

    // Exponential yields
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi", "pipipipi"};
    for (auto mode : modes) {
        m_pars->AddRealVar("n_expo_" + mode, GetMaxYield(mode)/2, 0, 
                GetMaxYield(mode));
    }

    // Four body floating parameters
    // Signal widths
    pr->ReadParameters("4body_signal", "signal_Kpipipi.param");

    // Signal widths
    m_pars->AddRealVar("4body_signal_sigma_L", 
            pr->GetValue("4body_signal", "sigma_L"),
            pr->GetValue("4body_signal", "sigma_L") - 10, 
            pr->GetValue("4body_signal", "sigma_L") + 10);
    m_pars->AddRealVar("4body_Bs_sigma_L", pr->GetValue("Bs", "sigma_L"),
            pr->GetValue("Bs", "sigma_L") - 10, 
            pr->GetValue("Bs", "sigma_L") + 10);

    // Helicity fraction for low mass shapes
    m_pars->AddRealVar("low_frac_010_Kpipipi", 0.5, 0, 1);
    m_pars->AddRealVar("low_frac_010_piKpipi", 0.5, 0, 1);
    m_pars->AddRealVar("low_frac_010_pipipipi", 0.5, 0, 1);

    // Slope of exponentials
    m_pars->AddRealVar("slope_Kpipipi", -0.005, -0.5, 0);
    m_pars->AddRealVar("slope_pipipipi", -0.005, -0.5, 0);

    // Signal yield ratios
    m_pars->AddRealVar("R_piKpipi_vs_Kpipipi" + extra_string, 0.06, 0, 3);
    m_pars->AddRealVar("R_pipipipi_vs_Kpipipi" + extra_string, 0.1, 0, 1);
    if (m_blind) {
        m_pars->AddUnblindVar("R_piKpipi_vs_Kpipipi", 
                "R_piKpipi_vs_Kpipipi_blind", "blind_piKpipi_ratio", 0.01);
        m_pars->AddUnblindVar("R_pipipipi_vs_Kpipipi", 
                "R_pipipipi_vs_Kpipipi_blind", "blind_pipipipi_ratio", 0.05);
    }

    // Other ratios
    m_pars->AddRealVar("R_pipipipi_vs_piKpipi_Bs", 0.1, 0, 1);
    m_pars->AddRealVar("R_piKpipi_vs_Kpipipi_low", 0.06, 0, 1);
    m_pars->AddRealVar("R_pipipipi_vs_Kpipipi_low", 0.1, 0, 1);

    // Floating yields
    m_pars->AddRealVar("n_signal_Kpipipi", 100, 0, GetMaxYield("Kpipipi"));
    m_pars->AddRealVar("n_Bs_piKpipi", 100, 0, GetMaxYield("piKpipi"));
    m_pars->AddRealVar("n_low_Kpipipi", 100, 0, GetMaxYield("Kpipipi"));
    m_pars->AddRealVar("n_Bs_low_piKpipi", 100, 0, GetMaxYield("piKpipi"));
    m_pars->AddRealVar("n_rho_Kpipipi", 20, 0, GetMaxYield("Kpipipi")/20);
    m_pars->AddRealVar("n_DKpipi_Kpipipi", 20, 0, GetMaxYield("Kpipipi")/5);
}
