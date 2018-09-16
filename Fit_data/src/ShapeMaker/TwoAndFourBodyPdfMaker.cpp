#include "TwoAndFourBodyPdfMaker.hpp"
#include "ParameterReader.hpp"

using namespace TwoAndFourBody;

// ===========
// Constructor
// ===========
TwoAndFourBodyPdfMaker::TwoAndFourBodyPdfMaker(RooRealVar * x, RooCategory * cat, bool blind) :
    TwoAndFourBodyBase("pdf", x, cat),
    m_blind(blind),
    m_hyp(Hypothesis::Signal) {}

TwoAndFourBodyPdfMaker::TwoAndFourBodyPdfMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, bool blind) : 
    TwoAndFourBodyBase(name, x, cat),
    m_blind(blind),
    m_hyp(Hypothesis::Signal) {}

TwoAndFourBodyPdfMaker::TwoAndFourBodyPdfMaker(RooRealVar * x, RooCategory * cat, 
       Hypothesis hyp, bool blind) :
    TwoAndFourBodyBase("pdf", x, cat),
    m_blind(blind),
    m_hyp(hyp) {}

TwoAndFourBodyPdfMaker::TwoAndFourBodyPdfMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, Hypothesis hyp, bool blind) : 
    TwoAndFourBodyBase(name, x, cat),
    m_blind(blind),
    m_hyp(hyp) {}



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
    std::vector<std::string> frac_types = {"Kpi", "piK", "GLW", "Kpipipi", 
        "piKpipi", "pipipipi"};
    for (auto type : frac_types) {
        m_pars->AddRealVar("low_frac_010_" + type, 0.5, 0, 1);
        if (type != "Kpi" && type != "Kpipipi") {
            m_pars->AddRealVar("low_frac_010_" + type + "_plus", 0.5, 0, 1);
            m_pars->AddRealVar("low_frac_010_" + type + "_minus", 0.5, 0, 1);
        }
    }
    m_pars->AddRealVar("Bs_low_frac_010", 0.5, 0, 1);

    // Slope of exponentials
    std::vector<std::string> expo_slopes = {"Kpi", "KK", "pipi", "Kpipipi", 
        "pipipipi"};
    for (auto slope : expo_slopes) {
        m_pars->AddRealVar("slope_" + slope, -0.005, -0.5, 0);
    }

    // Yield ratios
    std::string extra_string = m_blind ? "_blind" : "";
    std::vector<std::string> ratio_types = {extra_string, "_low", "_DKpipi"};
    for (auto type : ratio_types) {
        m_pars->AddRealVar("R_piK_vs_Kpi" + type, 0.06, 0, 1);
        m_pars->AddRealVar("R_KK_vs_Kpi" + type, 0.3, 0, 1);
        m_pars->AddRealVar("R_pipi_vs_Kpi" + type, 0.1, 0, 1);
        m_pars->AddRealVar("R_piKpipi_vs_Kpipipi" + type, 0.1, 0, 1);
        m_pars->AddRealVar("R_pipipipi_vs_Kpipipi" + type, 0.1, 0, 1);
    }

    // Make unblind versions of signal yields
    if (m_blind) {
        m_pars->AddUnblindVar("R_piK_vs_Kpi", "R_piK_vs_Kpi_blind", 
                "blind_piK_ratio", 0.01);
        m_pars->AddUnblindVar("R_KK_vs_Kpi", "R_KK_vs_Kpi_blind", 
                "blind_KK_ratio", 0.1);
        m_pars->AddUnblindVar("R_pipi_vs_Kpi", "R_pipi_vs_Kpi_blind", 
                "blind_pipi_ratio", 0.05);
        m_pars->AddUnblindVar("R_piKpipi_vs_Kpipipi", 
                "R_piKpipi_vs_Kpipipi_blind", "blind_piKpipi_ratio", 0.01);
        m_pars->AddUnblindVar("R_pipipipi_vs_Kpipipi", 
                "R_pipipipi_vs_Kpipipi_blind", "blind_pipipipi_ratio", 0.05);
    }

    // Bs ratios
    m_pars->AddRealVar("R_KK_vs_piK_Bs", 0.3, 0, 1);
    m_pars->AddRealVar("R_pipi_vs_piK_Bs", 0.1, 0, 1);
    m_pars->AddRealVar("R_pipipipi_vs_piKpipi_Bs", 0.1, 0, 1);

    // Floating yields
    m_pars->AddRealVar("n_signal_Kpi", GetMaxYield("Kpi")/3, 0, GetMaxYield("Kpi"));
    m_pars->AddRealVar("n_Bs_piK", GetMaxYield("piK")/3, 0, GetMaxYield("piK"));
    m_pars->AddRealVar("n_low_Kpi", GetMaxYield("Kpi")/2, 0, GetMaxYield("Kpi"));
    m_pars->AddRealVar("n_Bs_low_piK", GetMaxYield("piK")/3, 0, GetMaxYield("piK"));
    m_pars->AddRealVar("n_rho_Kpi", GetMaxYield("Kpi")/100, 0, GetMaxYield("Kpi")/20);
    m_pars->AddRealVar("n_DKpipi_Kpi", GetMaxYield("Kpi")/12, 0, GetMaxYield("Kpi")/5);

    // Asymmetries
    // Signal asymmtries
    m_pars->AddRealVar("A_Kpi", 0, -1, 1);
    m_pars->AddRealVar("A_KK" + extra_string, 0, -1, 1);
    m_pars->AddRealVar("A_pipi" + extra_string, 0, -1, 1);
    if (m_blind) {
        m_pars->AddUnblindVar("A_KK", "A_KK_blind", "blind_KK_asym", 0.01);
        m_pars->AddUnblindVar("A_pipi", "A_pipi_blind", "blind_pipi_asym", 0.01);
    }

    // Bs asymmetries
    std::vector<std::string> Bs_asym_modes = {"piK", "KK", "pipi", "piKpipi",
        "pipipipi"};
    for (auto asym : Bs_asym_modes) {
        m_pars->AddRealVar("A_" + asym + "_Bs", 0, -1, 1);
    }

    // Low mass asymmetries
    std::vector<std::string> asym_modes = {"Kpi", "KK", "pipi", "Kpipipi", 
        "pipipipi"};
    for (auto asym : asym_modes) {
        m_pars->AddRealVar("A_" + asym + "_low", 0, -1, 1);
        m_pars->AddRealVar("A_DKpipi_" + asym, 0, -1, 1);
    }

    // ADS mode ratios by flavour
    for (auto type : ratio_types) {
        m_pars->AddRealVar("R_plus" + type, 0.06, 0, 1);
        m_pars->AddRealVar("R_minus" + type, 0.06, 0, 1);
    }
    if (m_blind) {
        m_pars->AddUnblindVar("R_plus", "R_plus_blind", "blind_R_plus", 0.01);
        m_pars->AddUnblindVar("R_minus", "R_minus_blind", "blind_R_minus", 0.01);
    }

    // Exponential yields
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi", "pipipipi"};
    for (auto mode : modes) {
        m_pars->AddRealVar("n_expo_" + mode, GetMaxYield(mode)/4, 0, 
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

    // Ratio between four body and two body shapes
    m_pars->AddRealVar("four_vs_two_body_ratio", 1.06, 0.5, 1.5);

    // Floating yields
    m_pars->AddRealVar("n_signal_Kpipipi", GetMaxYield("Kpipipi")/3, 0, GetMaxYield("Kpipipi"));
    m_pars->AddRealVar("n_Bs_piKpipi", GetMaxYield("piKpipi")/3, 0, GetMaxYield("piKpipi"));
    m_pars->AddRealVar("n_low_Kpipipi", GetMaxYield("Kpipipi")/3, 0, GetMaxYield("Kpipipi"));
    m_pars->AddRealVar("n_Bs_low_piKpipi", GetMaxYield("piKpipi")/3, 0, GetMaxYield("piKpipi"));
    m_pars->AddRealVar("n_rho_Kpipipi", GetMaxYield("Kpipipi")/100, 0, GetMaxYield("Kpipipi")/20);
    m_pars->AddRealVar("n_DKpipi_Kpipipi", GetMaxYield("Kpipipi")/12, 0, GetMaxYield("Kpipipi")/5);

    // Four body asymmetries
    // Signal asymmtries
    m_pars->AddRealVar("A_Kpipipi", 0, -1, 1);
    m_pars->AddRealVar("A_pipipipi" + extra_string, 0, -1, 1);
    if (m_blind) { 
        m_pars->AddUnblindVar("A_pipipipi", "A_pipipipi_blind", 
                "blind_pipipipi_asym", 0.01);
    }

    // ADS mode asymmetries
    m_pars->AddRealVar("R_plus_4body" + extra_string, 0.06, 0, 1);
    m_pars->AddRealVar("R_minus_4body" + extra_string, 0.06, 0, 1);
    if (m_blind) {
        m_pars->AddUnblindVar("R_plus_4body", "R_plus_4body_blind",
                "blind_R_plus_4body", 0.01);
        m_pars->AddUnblindVar("R_minus_4body", "R_minus_4body_blind",
                "blind_R_minus_4body", 0.01);
    }
    m_pars->AddRealVar("R_plus_low_4body", 0.06, 0, 1);
    m_pars->AddRealVar("R_minus_low_4body", 0.06, 0, 1);
    m_pars->AddRealVar("R_plus_DKpipi_4body", 0.06, 0, 1);
    m_pars->AddRealVar("R_minus_DKpipi_4body", 0.06, 0, 1);

    // Fix ratios if using a null hypothesis
    m_pars->SetWarnings(false);
    if (m_hyp == Hypothesis::NullTwoBody) {
        m_pars->AddRealVar("R_piK_vs_Kpi", 0);
    } else if (m_hyp == Hypothesis::NullFourBody) {
        m_pars->AddRealVar("R_piKpipi_vs_Kpipipi", 0);
    }
    m_pars->SetWarnings(true);

}