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
    pr->ReadParameters("Bs", "signal_Bs.param");
    pr->ReadParameters("rho", "rho_all_PIDcut.param");
    pr->ReadParameters("low", "lowMass.param");
    pr->ReadParameters("DKpipi", "DKpipi.param");

    // Difference between B0 and Bs masses
    m_pars->AddRealVar("delta_M", 87.26);

    // Crystal ball shape parameters (signal shapes, rho mis-ID)
    std::vector<std::string> cb_types = {"signal", "Bs", "rho"};
    std::vector<std::string> cb_pars = {"sigma_ratio", "alpha_L", "alpha_R", "n_L", 
        "n_R", "frac"};
    for (auto cb : cb_types) {
        if (cb != "Bs") m_pars->AddRealVar(cb + "_mean_before_shift", 
                pr->GetValue(cb, "mean"));
        if (cb == "rho") m_pars->AddRealVar(cb + "_sigma_L", 
                pr->GetValue(cb, "sigma_L"));
        for (auto cb_par : cb_pars) {
            m_pars->AddRealVar(cb + "_" + cb_par, pr->GetValue(cb, cb_par));
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
    m_pars->AddRealVar("shiftg", 0);

    // Other low mass parameters
    std::vector<std::string> low_types = {"gamma_010", "gamma_101", "pi_010", "pi_101"};
    std::vector<std::string> low_pars = {"sigma", "csi"};
    for (auto par : low_pars) {
        for (auto type : low_types) {
            m_pars->AddRealVar("low_" + par + "_" + type, 
                    pr->GetValue("low", par + "_" + type));
            m_pars->AddRealVar("Bs_low_" + par + "_" + type, 
                    pr->GetValue("low", "Bs_" + par + "_" + type));
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

    // B0 yields from ratios
    std::vector<std::string> B0_yield_shapes = {"piK", "pipi", "KK"};
    for (auto shape : B0_yield_shapes) {
        m_pars->AddProductVar("n_signal_" + shape, "n_signal_Kpi", 
                "R_" + shape + "_vs_Kpi");
        m_pars->AddProductVar("n_low_" + shape, "n_low_Kpi", 
                "R_" + shape + "_vs_Kpi_low");
    }

    // Bs yields from ratios
    std::vector<std::string> Bs_yield_shapes = {"pipi", "KK"};
    for (auto shape : Bs_yield_shapes) {
        m_pars->AddProductVar("n_Bs_" + shape, "n_Bs_piK", 
                "R_" + shape + "_vs_piK_Bs");
        m_pars->AddProductVar("n_Bs_low_" + shape, "n_Bs_low_piK", 
                "R_" + shape + "_vs_piK_Bs_low");
    }

    // Rho yields: use fixed Bs low mass ratios
    m_pars->AddShared("n_rho_piK", "n_rho_Kpi");
    m_pars->AddProductVar("n_rho_KK", "n_rho_piK", "R_KK_vs_piK_Bs_low");
    m_pars->AddProductVar("n_rho_pipi", "n_rho_piK", "R_pipi_vs_piK_Bs_low");

}


// =========================
// Make each component shape
// =========================
void TwoBodyBase::MakeComponentShapes() {

    // Double crystal ball shapes (B0 and Bs signal, rho mis-ID)
    std::vector<std::string> cb_shapes = {"signal", "rho", "Bs"};
    for (auto cb : cb_shapes) {
        m_shapes->AddCrystalBall(cb + "_CB_L", cb + "_mean", cb + "_sigma_L",
                cb + "_alpha_L", cb + "_n_L");
        m_shapes->AddCrystalBall(cb + "_CB_R", cb + "_mean", cb + "_sigma_R",
                cb + "_alpha_R", cb + "_n_R");
        m_shapes->CombineShapes(cb, cb + "_CB_L", cb + "_CB_R", cb + "_frac");
    }

    // Low mass shapes
    std::vector<std::string> low_prefix = {"", "Bs_"};
    for (auto pre : low_prefix) {

        // Make each shape
        m_shapes->AddHill(pre + "gamma_010_shape", pre + "low_a_gamma", 
                pre + "low_b_gamma", pre + "low_csi_gamma_010", "shift", 
                pre + "low_sigma_gamma_010", "low_ratio", "low_frac");
        m_shapes->AddLittleHorns(pre + "gamma_101_shape", pre + "low_a_gamma", 
                pre + "low_b_gamma", pre + "low_csi_gamma_101", "shift", 
                pre + "low_sigma_gamma_101", "low_ratio", "low_frac", "shiftg");
        m_shapes->AddHorns(pre + "pi_010_shape", pre + "low_a_pi", 
                pre + "low_b_pi", pre + "low_csi_pi_010", "shift", 
                pre + "low_sigma_pi_010", "low_ratio", "low_frac");
        m_shapes->AddHill(pre + "pi_101_shape", pre + "low_a_pi", 
                pre + "low_b_pi", pre + "low_csi_pi_101", "shift", 
                pre + "low_sigma_pi_101", "low_ratio", "low_frac");

        // Combine into helicity shapes
        m_shapes->CombineShapes(pre + "low_010_shape", {
                {pre + "gamma_010_shape", "coeff_gamma_010"},
                {pre + "pi_010_shape", "coeff_pi_010"}});
        m_shapes->CombineShapes(pre + "low_101_shape", {
                {pre + "gamma_101_shape", "coeff_gamma_101"},
                {pre + "pi_101_shape", "coeff_pi_101"}});
    }

    // Combine the helicity components
    m_shapes->CombineShapes("low_Kpi", "low_010_shape", "low_101_shape",
            "low_frac_010_Kpi");
    m_shapes->CombineShapes("low_piK", "low_010_shape", "low_101_shape",
            "low_frac_010_piK");
    m_shapes->CombineShapes("low_KK", "low_010_shape", "low_101_shape",
            "low_frac_010_GLW");
    m_shapes->AddShared("low_pipi", "low_KK");
    m_shapes->CombineShapes("Bs_low", "Bs_low_010_shape", "Bs_low_101_shape",
            "Bs_low_frac_010");

    // DKpipi shape here

    // Combinatorial shapes
    m_shapes->AddExponential("expo_Kpi", "slope_Kpi");
    m_shapes->AddExponential("expo_piK", "slope_Kpi");
    m_shapes->AddExponential("expo_KK", "slope_KK");
    m_shapes->AddExponential("expo_pipi", "slope_pipi");
}


// ==============================
// Make total shape for each mode
// ==============================
void TwoBodyBase::MakeModeShapes() {

    for (auto mode : m_modes) {

        // Map of shapes and yields
        std::map<std::string, std::string> shapes;

        // Shapes common to all modes
        shapes.emplace("signal", "n_signal_" + mode);
        shapes.emplace("expo_" + mode, "n_expo_" + mode);
        shapes.emplace("low_" + mode, "n_low_" + mode);
        shapes.emplace("rho", "n_rho_" + mode);

        // Shapes for all except Kpi
        if (mode != "Kpi") {
            shapes.emplace("Bs", "n_Bs_" + mode);
            shapes.emplace("Bs_low", "n_Bs_low_" + mode);
        }

        // Make the shape
        m_shapes->CombineShapes(mode, shapes);
    }
}

