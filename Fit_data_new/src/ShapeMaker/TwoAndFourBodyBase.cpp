#include "ParameterReader.hpp"
#include "TwoAndFourBodyBase.hpp" 

// ===========
// Constructor
// ===========
TwoAndFourBodyBase::TwoAndFourBodyBase(std::string name, RooRealVar * x, RooCategory * cat) :
    TwoBodyBase(name, x, cat) {}


// =======================
// Set constant parameters
// =======================
void TwoAndFourBodyBase::SetConstantParameters() {

    // Set two body constant parameters
    TwoBodyBase::SetConstantParameters();

    // Extra four body constant parameters
    // Four body signal shape
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Fit_monte_carlo/Results/");
    pr->ReadParameters("4body_signal", "signal_Kpipipi.param");
    m_pars->AddRealVar("4body_signal_mean_before_shift", 
            pr->GetValue("4body_signal", "mean"));
    std::vector<std::string> cb_pars = {"sigma_ratio", "alpha_L", "alpha_R", 
        "n_L", "n_R", "frac"};
    for (auto cb_par : cb_pars) {
        m_pars->AddRealVar("4body_signal_" + cb_par, pr->GetValue("4body_signal", 
                    cb_par));
    }

    // Ratio between low mass Bs for pipipipi and piKpipipi
    pr->ReadParameters("ratios", "../../Parameters/low_mass_Bs_ratio.param");
    m_pars->AddRealVar("R_pipipipi_vs_piKpipi_Bs_low", pr->GetValue("ratios", 
                "pipipipi_vs_piKpipi"));
}

// ==========================================
// Set parameters with dependencies on others
// ==========================================
void TwoAndFourBodyBase::SetDependentParameters() {

    // Set two body dependent parameters
    TwoBodyBase::SetDependentParameters();

    // Extra four body dependent parameters
    // Signal mean and width
    m_pars->AddSummedVar("4body_signal_mean", "4body_signal_mean_before_shift", 
            "shift");
    m_pars->AddProductVar("4body_signal_sigma_R", "4body_signal_sigma_L", 
            "4body_signal_sigma_ratio");

    // Bs mean and width
    m_pars->AddSummedVar("4body_Bs_mean", "4body_signal_mean", "delta_M");
    m_pars->AddProductVar("4body_Bs_sigma_R", "4body_Bs_sigma_L", "Bs_sigma_ratio");

    // B0 yields from ratios
    std::vector<std::string> B0_yield_shapes = {"piKpipi", "pipipipi"};
    for (auto shape : B0_yield_shapes) {
        m_pars->AddProductVar("n_signal_" + shape, "n_signal_Kpipipi", 
                "R_" + shape + "_vs_Kpipipi");
        m_pars->AddProductVar("n_low_" + shape, "n_low_Kpipipi", 
                "R_" + shape + "_vs_Kpipipi_low");
    }

    // Bs yields from ratios
    m_pars->AddProductVar("n_Bs_pipipipi", "n_Bs_piK", "R_pipipipi_vs_piKpipi_Bs");
    m_pars->AddProductVar("n_Bs_low_pipipipi", "n_Bs_low_piK", 
        "R_pipipipi_vs_piKpipi_Bs_low");

    // Rho yields: use fixed Bs low mass ratios
    m_pars->AddShared("n_rho_piKpipi", "n_rho_Kpipipi");
    m_pars->AddProductVar("n_rho_pipipipi", "n_rho_piKpipi", 
        "R_pipipipi_vs_piKpipi_Bs_low");

}


// =========================
// Make each component shape
// =========================
void TwoAndFourBodyBase::MakeComponentShapes() {

    // Make two body components
    TwoBodyBase::MakeComponentShapes();

    // Four body signal shape
    m_shapes->AddCrystalBall("4body_signal_CB_L", "4body_signal_mean", 
            "4body_signal_sigma_L", "4body_signal_alpha_L", "4body_signal_n_L");
    m_shapes->AddCrystalBall("4body_signal_CB_R", "4body_signal_mean", 
            "4body_signal_sigma_R", "4body_signal_alpha_R", "4body_signal_n_R");
    m_shapes->CombineShapes("4body_signal", "4body_signal_CB_L", 
            "4body_signal_CB_R", "4body_signal_frac");

    // Four body Bs shape
    m_shapes->AddCrystalBall("4body_Bs_CB_L", "4body_Bs_mean", "4body_Bs_sigma_L",
            "Bs_alpha_L", "Bs_n_L");
    m_shapes->AddCrystalBall("4body_Bs_CB_R", "4body_Bs_mean", "4body_Bs_sigma_R",
            "Bs_alpha_R", "Bs_n_R");
    m_shapes->CombineShapes("4body_Bs", "4body_Bs_CB_L", "4body_Bs_CB_R",
            "Bs_frac");

    // Low mass shapes
    m_shapes->CombineShapes("low_Kpipipi", "low_010_shape", "low_101_shape",
            "low_frac_010_Kpipipi");
    m_shapes->CombineShapes("low_piKpipi", "low_010_shape", "low_101_shape",
            "low_frac_010_piKpipi");
    m_shapes->CombineShapes("low_pipipipi", "low_010_shape", "low_101_shape",
            "low_frac_010_pipipipi");

    // Combinatorial shapes
    m_shapes->AddExponential("expo_Kpipipi", "slope_Kpipipi");
    m_shapes->AddExponential("expo_piKpipi", "slope_Kpipipi");
    m_shapes->AddExponential("expo_pipipipi", "slope_pipipipi");
}


// ==============================
// Make total shape for each mode
// ==============================
void TwoAndFourBodyBase::MakeModeShapes() {

    for (auto mode : m_modes) {

        // Map of shapes and yields
        std::map<std::string, std::string> shapes;

        // Shapes common to all modes
        if (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") {
            shapes.emplace("4body_signal", "n_signal_" + mode);
        } else {
            shapes.emplace("signal", "n_signal_" + mode);
        }
        shapes.emplace("expo_" + mode, "n_expo_" + mode);
        shapes.emplace("low_" + mode, "n_low_" + mode);
        shapes.emplace("rho", "n_rho_" + mode);

        // Shapes for all except Kpi
        if (mode != "Kpi" && mode != "Kpipipi") {
            if (mode == "piKpipi" || mode == "pipipipi") {
                shapes.emplace("4body_Bs", "n_Bs_" + mode);
            } else {
                shapes.emplace("Bs", "n_Bs_" + mode);
            }
            shapes.emplace("Bs_low", "n_Bs_low_" + mode);
        } else if (mode == "Kpi") {
            shapes.emplace("DKpipi", "n_DKpipi_" + mode);
        }

        // Make the shape
        m_shapes->CombineShapes(mode, shapes);
        m_shapes->Get(mode)->Print("v");
    }
}
