#include "ParameterReader.hpp"
#include "TwoBodyBase.hpp" 

// ===========
// Constructor
// ===========
TwoBodyBase::TwoBodyBase(std::string name, RooRealVar * x, RooCategory * cat) :
    ShapeMakerBase(name, x, cat) {}


// =======================
// Set constant parameters
// =======================
void TwoBodyBase::SetConstantParameters() {

    // Read in MC fit parameters
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Fit_monte_carlo/Results/");
    pr->ReadParameters("signal", "signal_Kpi.param");
    pr->ReadParameters("Bs", "Bs.param");
    pr->ReadParameters("rho", "rho.param");
    pr->ReadParameters("low", "lowMass.param");
    pr->ReadParameters("DKpipi", "DKpipi.param");

    // Difference between B0 and Bs masses
    m_pars->AddRealVar("delta_M", 87.26);

    // Crystal ball shape parameters (signal shapes, rho mis-ID)
    std::vector<std::string> cb_types = {"signal", "Bs", rho"};
    std::vector<std::string> cb_pars = {"sigma_ratio", "alpha_L", "alpha_R", "n_L", 
        "n_R", "frac"};
    for (auto cb : cb_types) {
        if (cb != "Bs") m_pars->AddRealVar(cb + "_mean_before_shift", 
                pr->GetValue(cb, "mean"));
        for (auto cb_par : cb_pars) {
            m_pars->AddRealVar(cb + "_" + cb_pars, pr->GetValue(cb, cb_par));
        }
    }

    // Low mass parameters
    // Kinematic endpoints
    std::vector<std::string> low_endpoints = {"a_gamma", "b_gamma", "a_pi", "b_pi"};
    for (auto ep : low_endpoints) {
        m_pars->AddRealVar("low_" + ep, pr->GetValue("low", ep));
    }

    // Shared parameters
    m_pars->AddRealVar("low_ratio", pr->GetValue("low", "ratio"));
    m_pars->AddRealVar("low_frac", pr->GetValue("low", "frac"));
    m_pars->AddRealVar("low_shiftg", 0);

    // Other low mass parameters
    std::vector<std::string> low_pars = {"gamma_010", "gamma_101", "pi_010", "pi_101"};
    std::vector<std::string> low_types = {"sigma", "csi", "Bs_sigma", "Bs_csi"};
    for (auto par : low_pars) {
        for (auto type : low_types) {
            m_pars->AddRealVar("low_" + par + "_" + type, 
                    pr->GetValue("low", par + "_" + type));
        }
    }

    // Gamma vs. pi0 ratio
    pr->ReadParameters("gamma_pi", "../../Parameters/gamma_vs_pi.param");
    const double G_010 = pr->GetValue("gamma_pi", "br_gamma") * 
        pr->GetValue("gamma_pi", "acc_eff_gamma") * 
        pr->GetValue("gamma_pi", "sel_eff_gamma_010");
    const double G_101 = pr->GetValue("gamma_pi", "br_gamma") * 
        pr->GetValue("gamma_pi", "acc_eff_gamma") * 
        pr->GetValue("gamma_pi", "sel_eff_gamma_101");
    const double P_010 = pr->GetValue("gamma_pi", "br_pi") * 
        pr->GetValue("gamma_pi", "acc_eff_pi") * 
        pr->GetValue("gamma_pi", "sel_eff_pi_010");
    const double P_101 = pr->GetValue("gamma_pi", "br_pi") * 
        pr->GetValue("gamma_pi", "acc_eff_pi") * 
        pr->GetValue("gamma_pi", "sel_eff_pi_101");
    m_pars->AddRealVar("coeff_gamma_010", G_010/(G_010 + P_010));
    m_pars->AddRealVar("coeff_gamma_101", G_101/(G_101 + P_101));
    m_pars->AddFormulaVar("coeff_pi_010", "1 - @1", ParameterList("coeff_gamma_010"));
    m_pars->AddFormulaVar("coeff_pi_101", "1 - @1", ParameterList("coeff_gamma_101"));

    // Bs low mass ratios
    pr->ReadParameters("ratio", "../../Parameters/low_mass_Bs_ratio.param");
    m_pars->AddRealVar("R_KK_vs_piK_Bs_low", pr->GetValue("ratio", "KK_vs_piK"));
    m_pars->AddRealVar("R_pipi_vs_piK_Bs_low", pr->GetValue("ratio", "pipi_vs_piK"));

    // DKpipi shape parameters
    std::vector<std::string> DKpipi_comps = {"1a", "2", "3", "5a"};
    std::vector<std::string> DKpipi_pars = {"a", "b", "csi", "sigma", "ratio",
        "frac", "coeff"};
    for (auto comp : DKpipi_comps) {
        for (auto par : DKpipi_pars) {
            m_pars->AddRealVar("DKpipi_" + par + "_" + comp, 
                    pr->GetValue("DKpipi", par + "_" + comp));
        }
    }
}


// ==========================================
// Set parameters with dependencies on others
// ==========================================
void TwoBodyBase::SetDependentParameters() {

    // Shifted parameters
    m_pars->AddSummedVar("signal_mean", "signal_mean_before_shift", "shift");
    m_pars->AddSummedVar("rho_mean", "rho_mean_before_shift", "shift");
    m_pars->AddSummedVar("Bs_mean", "signal_mean", "delta_M");

    // Widths from ratios
    m_pars->AddProductVar("signal_sigma_R", "signal_sigma_L", "signal_sigma_ratio");
    m_pars->AddProductVar("Bs_sigma_R", "Bs_sigma_L", "Bs_sigma_ratio");
    m_pars->AddProductVar("rho_sigma_R", "rho_sigma_L", "rho_sigma_ratio");

    // Bs low mass kinematic endpoints
    m_pars->AddSummedVar("Bs_low_a_gamma", "low_a_gamma", "delta_M");
    m_pars->AddSummedVar("Bs_low_b_gamma", "low_b_gamma", "delta_M");
    m_pars->AddSummedVar("Bs_low_a_pi", "low_a_pi", "delta_M");
    m_pars->AddSummedVar("Bs_low_b_pi", "low_b_pi", "delta_M");
}


// =========================
// Make each component shape
// =========================
void TwoBodyBase::MakeComponentShapes() {
}


// ==============================
// Make total shape for each mode
// ==============================
void TwoBodyBase::MakeModeShapes() {
}

