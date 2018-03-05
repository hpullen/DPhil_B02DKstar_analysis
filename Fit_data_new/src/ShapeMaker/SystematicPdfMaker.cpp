#include "SystematicPdfMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
SystematicPdfMaker::SystematicPdfMaker(RooRealVar * x, RooCategory * cat,
        bool blind) : TwoAndFourBodyPdfMaker("sys", x, cat, blind) {}


// ==========
// Destructor
// ==========
SystematicPdfMaker::~SystematicPdfMaker() {}


// ========================================
// Override of constant parameters function
// ========================================
void SystematicPdfMaker::SetConstantParameters() {

    // Read in MC fit parameters
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Fit_monte_carlo/Results/");
    pr->ReadParameters("signal", "signal_Kpi.param");
    pr->ReadParameters("Bs", "signal_Bs.param");
    pr->ReadParameters("rho", "rho_all_PIDcut.param");
    pr->ReadParameters("low", "lowMass.param");
    pr->ReadParameters("DKpipi", "DKpipi.param");

    // Difference between B0 and Bs masses
    m_pars->AddRandomisedVar("delta_M", 87.26, 0.24);

    // Crystal ball shape parameters (signal shapes, rho mis-ID)
    std::vector<std::string> cb_types = {"signal", "Bs", "rho"};
    std::vector<std::string> cb_pars = {"sigma_ratio", "alpha_L", "alpha_R", "n_L", 
        "n_R", "frac"};
    for (auto cb : cb_types) {
        if (cb != "Bs") m_pars->AddRandomisedVar(cb + "_mean_before_shift", 
                pr->GetValue(cb, "mean"), pr->GetError(cb, "mean"));
        if (cb == "rho") m_pars->AddRandomisedVar(cb + "_sigma_L", 
                pr->GetValue(cb, "sigma_L"), pr->GetError(cb, "sigma_L"));
        for (auto cb_par : cb_pars) {
            m_pars->AddRandomisedVar(cb + "_" + cb_par, pr->GetValue(cb, cb_par), 
                    pr->GetError(cb, cb_par));
        }
    }

    // Low mass parameters
    // Kinematic endpoints
    std::vector<std::string> low_endpoints = {"a_gamma", "b_gamma", "a_pi", "b_pi"};
    for (auto ep : low_endpoints) {
        m_pars->AddRandomisedVar("low_" + ep, pr->GetValue("low", ep), 
                pr->GetError("low", ep));
    }

    // Shared parameters
    m_pars->AddRandomisedVar("low_ratio", pr->GetValue("low", "ratio"),
            pr->GetError("low", "ratio"));
    m_pars->AddRandomisedVar("low_frac", pr->GetValue("low", "frac"), 
            pr->GetError("low", "frac"));
    m_pars->AddRealVar("shiftg", 0);

    // Other low mass parameters
    std::vector<std::string> low_types = {"gamma_010", "gamma_101", "pi_010", "pi_101"};
    std::vector<std::string> low_pars = {"sigma", "csi"};
    for (auto par : low_pars) {
        for (auto type : low_types) {
            m_pars->AddRandomisedVar("low_" + par + "_" + type, 
                    pr->GetValue("low", par + "_" + type),
                    pr->GetError("low", par + "_" + type));
            m_pars->AddRandomisedVar("Bs_low_" + par + "_" + type, 
                    pr->GetValue("low", "Bs_" + par + "_" + type),
                    pr->GetError("low", "Bs_" + par + "_" + type));
        }
    }

    // Gamma vs. pi0 ratio
    pr->ReadParameters("gamma_pi", "../../Parameters/gamma_vs_pi.param");

    // Get selection efficiencies
    m_pars->AddRealVar("sel_eff_gamma_010", 
            pr->GetValue("gamma_pi", "sel_eff_gamma_010"));
    m_pars->AddRealVar("sel_eff_gamma_101", 
            pr->GetValue("gamma_pi", "sel_eff_gamma_101"));
    m_pars->AddRealVar("sel_eff_pi_010", 
            pr->GetValue("gamma_pi", "sel_eff_pi_010"));
    m_pars->AddRealVar("sel_eff_pi_101", 
            pr->GetValue("gamma_pi", "sel_eff_pi_101"));

    // Get acceptance efficiencies
    m_pars->AddRealVar("acc_eff_gamma", 
            pr->GetValue("gamma_pi", "acc_eff_gamma"));
    m_pars->AddRealVar("acc_eff_pi", 
            pr->GetValue("gamma_pi", "acc_eff_pi"));

    // Get branching fractions
    m_pars->AddRealVar("br_gamma", pr->GetValue("gamma_pi", "br_gamma"));
    m_pars->AddRealVar("br_pi", pr->GetValue("gamma_pi", "br_pi"));

    // Multiply together
    m_pars->AddFormulaVar("G_010", "@0 * @1 * @2", 
            ParameterList("br_gamma", "acc_eff_gamma", "sel_eff_gamma_010"));
    m_pars->AddFormulaVar("G_101", "@0 * @1 * @2", 
            ParameterList("br_gamma", "acc_eff_gamma", "sel_eff_gamma_101"));
    m_pars->AddFormulaVar("P_010", "@0 * @1 * @2", 
            ParameterList("br_pi", "acc_eff_pi", "sel_eff_pi_010"));
    m_pars->AddFormulaVar("P_101", "@0 * @1 * @2", 
            ParameterList("br_pi", "acc_eff_pi", "sel_eff_pi_101"));

    // Make coefficients
    m_pars->AddFormulaVar("coeff_gamma_010", "@0/(@0 + @1)", 
            ParameterList("G_010", "P_010"));
    m_pars->AddFormulaVar("coeff_gamma_101", "@0/(@0 + @1)", 
            ParameterList("G_101", "P_101"));
    m_pars->AddFormulaVar("coeff_pi_010", "1 - @0", ParameterList("coeff_gamma_010"));
    m_pars->AddFormulaVar("coeff_pi_101", "1 - @0", ParameterList("coeff_gamma_101"));

    // Bs low mass ratios
    pr->ReadParameters("ratio", "../../Parameters/low_mass_Bs_ratio.param");
    m_pars->AddRealVar("R_KK_vs_piK_Bs_low", pr->GetValue("ratio", "KK_vs_piK"));
    m_pars->AddRealVar("R_pipi_vs_piK_Bs_low", pr->GetValue("ratio", "pipi_vs_piK"));

    // DKpipi shape parameters
    std::vector<std::string> DKpipi_comps = {"1a", "2", "3", "5a"};
    std::vector<std::string> DKpipi_pars = {"a", "b", "csi", "sigma", "ratio",
        "frac", "coeff"};
    bool inc_DKpipi = false;
    for (auto comp : DKpipi_comps) {
        for (auto par : DKpipi_pars) {
            if (inc_DKpipi) {
                if (par == "coeff") {
                    m_pars->AddRealVar("DKpipi_" + par + "_" + comp, 
                            pr->GetValue("DKpipi", par + "_" + comp));
                } else {
                    m_pars->AddRandomisedVar("DKpipi_" + par + "_" + comp, 
                            pr->GetValue("DKpipi", par + "_" + comp),
                            pr->GetError("DKpipi", par + "_" + comp));
                }
            } else {
                m_pars->AddRealVar("DKpipi_" + par + "_" + comp, 
                        pr->GetValue("DKpipi", par + "_" + comp));
            }
        }
    }

    // Extra four body constant parameters
    // Four body signal shape
    pr->ReadParameters("4body_signal", "signal_Kpipipi.param");
    m_pars->AddRandomisedVar("4body_signal_mean_before_shift", 
            pr->GetValue("4body_signal", "mean"),
            pr->GetError("4body_signal", "mean"));
    for (auto cb_par : cb_pars) {
        m_pars->AddRandomisedVar("4body_signal_" + cb_par, 
                pr->GetValue("4body_signal", cb_par),
                pr->GetError("4body_signal", cb_par));
    }
    

    // Ratio between low mass Bs for pipipipi and piKpipipi
    pr->ReadParameters("ratios", "../../Parameters/low_mass_Bs_ratio.param");
    m_pars->AddRealVar("R_pipipipi_vs_piKpipi_Bs_low", 
            pr->GetValue("ratios", "pipipipi_vs_piKpipi"));
}
