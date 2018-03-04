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

    // Four body widths scaled by ratio
    std::vector<std::string> widths_to_scale = {"low_sigma_gamma_010",
        "low_sigma_gamma_101", "low_sigma_pi_010", "low_sigma_pi_101", 
        "Bs_low_sigma_gamma_010", "Bs_low_sigma_gamma_101", "Bs_low_sigma_pi_010", 
        "Bs_low_sigma_pi_101", "DKpipi_sigma_1a", "DKpipi_sigma_2", 
        "DKpipi_sigma_3", "DKpipi_sigma_5a", "rho_sigma_L", "rho_sigma_R"};
    for (auto width : widths_to_scale) {
        m_pars->AddProductVar("4body_" + width, width, "four_vs_two_body_ratio");
    }

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

    // Split signal/low mass yields using asymmetries
    if (IsSplit()) {
        std::vector<std::string> B0_asym_modes = {"Kpipipi", "pipipipi"};
        std::vector<std::string> Bs_asym_modes = {"piKpipi", "pipipipi"};
        for (auto mode : B0_asym_modes) {
            m_pars->AddFormulaVar("a_" + mode, "(1 + @0)/(1 - @0)", 
                    ParameterList("A_" + mode ));
            m_pars->AddFormulaVar("n_signal_" + mode + "_plus", "@0/(1 + @1)",
                    ParameterList("n_signal_" + mode, "a_" + mode));
            m_pars->AddFormulaVar("n_signal_" + mode + "_minus", "@0/(1 + 1/@1)",
                    ParameterList("n_signal_" + mode, "a_" + mode));
            m_pars->AddFormulaVar("a_" + mode + "_low", "(1 + @0)/(1 - @0)", 
                    ParameterList("A_" + mode + "_low"));
            m_pars->AddFormulaVar("n_low_" + mode + "_plus", "@0/(1 + @1)",
                    ParameterList("n_low_" + mode, "a_" + mode + "_low"));
            m_pars->AddFormulaVar("n_low_" + mode + "_minus", "@0/(1 + 1/@1)",
                    ParameterList("n_low_" + mode, "a_" + mode + "_low"));
        }
        for (auto mode : Bs_asym_modes) {
            m_pars->AddFormulaVar("a_" + mode + "_Bs", "(1 + @0)/(1 - @0)", 
                    ParameterList("A_" + mode + "_Bs"));
            m_pars->AddFormulaVar("n_Bs_" + mode + "_plus", "@0/(1 + 1/@1)",
                    ParameterList("n_Bs_" + mode, "a_" + mode + "_Bs"));
            m_pars->AddFormulaVar("n_Bs_" + mode + "_minus", "@0/(1 + @1)",
                    ParameterList("n_Bs_" + mode, "a_" + mode + "_Bs"));
        }
        // Suppressed ADS mode yields
        m_pars->AddProductVar("n_signal_piKpipi_plus", "n_signal_Kpipipi_plus", 
                "R_plus_4body");
        m_pars->AddProductVar("n_signal_piKpipi_minus", "n_signal_Kpipipi_minus", 
                "R_minus_4body");
        m_pars->AddProductVar("n_low_piKpipi_plus", "n_low_Kpipipi_plus", 
                "R_plus_low_4body");
        m_pars->AddProductVar("n_low_piKpipi_minus", "n_low_Kpipipi_minus", 
                "R_minus_low_4body");

        // Split rho and exponential yields equally in all modes
        std::vector<std::string> all_modes = {"Kpipipi", "piKpipi", "pipipipi"};
        std::vector<std::string> even_split = {"expo", "rho"};
        for (auto mode : all_modes) {
            for (auto spl : even_split) {
                m_pars->AddFormulaVar("n_" + spl + "_" + mode + "_plus", "@0/2", 
                        ParameterList("n_" + spl + "_" + mode));
                m_pars->AddShared("n_" + spl + "_" + mode + "_minus", 
                        "n_" + spl + "_" + mode + "_plus");
            }

            // Split Bs low equally
            if (mode != "Kpipipi") {
                m_pars->AddFormulaVar("n_Bs_low_" + mode + "_plus", "@0/2",
                        ParameterList("n_Bs_low_" + mode));
                m_pars->AddShared("n_Bs_low_" + mode + "_minus", 
                        "n_Bs_low_" + mode + "_plus");
            }
        }
    }

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

    // Rho mis-ID shape
    m_shapes->AddCrystalBall("4body_rho_CB_L", "rho_mean", "4body_rho_sigma_L",
            "rho_alpha_L", "rho_n_L");
    m_shapes->AddCrystalBall("4body_rho_CB_R", "rho_mean", "4body_rho_sigma_R",
            "rho_alpha_R", "rho_n_R");
    m_shapes->CombineShapes("4body_rho", "4body_rho_CB_L", "4body_rho_CB_R",
            "rho_frac");

    // Low mass components
    std::vector<std::string> low_prefix = {"", "Bs_"};
    for (auto pre : low_prefix) {

        // Make each shape
        m_shapes->AddHill("4body_" + pre + "gamma_010_shape", pre + "low_a_gamma", 
                pre + "low_b_gamma", pre + "low_csi_gamma_010", "shift", 
                "4body_" + pre + "low_sigma_gamma_010", "low_ratio", "low_frac");
        m_shapes->AddLittleHorns("4body_" + pre + "gamma_101_shape", 
                pre + "low_a_gamma", pre + "low_b_gamma", 
                pre + "low_csi_gamma_101", "shift", 
                "4body_" + pre + "low_sigma_gamma_101", "low_ratio", "low_frac", 
                "shiftg");
        m_shapes->AddHorns("4body_" + pre + "pi_010_shape", pre + "low_a_pi", 
                pre + "low_b_pi", pre + "low_csi_pi_010", "shift", 
                "4body_" + pre + "low_sigma_pi_010", "low_ratio", "low_frac");
        m_shapes->AddHill("4body_" + pre + "pi_101_shape", pre + "low_a_pi", 
                pre + "low_b_pi", pre + "low_csi_pi_101", "shift", 
                "4body_" + pre + "low_sigma_pi_101", "low_ratio", "low_frac");

        // Combine into helicity shapes
        m_shapes->CombineShapes("4body_" + pre + "low_010_shape", {
                {"4body_" + pre + "gamma_010_shape", "coeff_gamma_010"},
                {"4body_" + pre + "pi_010_shape", "coeff_pi_010"}});
        m_shapes->CombineShapes("4body_" + pre + "low_101_shape", {
                {"4body_" + pre + "gamma_101_shape", "coeff_gamma_101"},
                {"4body_" + pre + "pi_101_shape", "coeff_pi_101"}});
    }

    // Low mass shapes
    m_shapes->CombineShapes("low_Kpipipi", "4body_low_010_shape", 
            "4body_low_101_shape", "low_frac_010_Kpipipi");
    m_shapes->CombineShapes("low_piKpipi", "4body_low_010_shape", 
            "4body_low_101_shape", "low_frac_010_piKpipi");
    m_shapes->CombineShapes("low_pipipipi", "4body_low_010_shape", 
            "4body_low_101_shape", "low_frac_010_pipipipi");
    m_shapes->CombineShapes("4body_Bs_low", "4body_Bs_low_010_shape", 
            "4body_Bs_low_101_shape", "Bs_low_frac_010");

    // DKpipi background
    std::vector<std::string> DKpipi_horns = {"1a", "3", "5a"};
    for (auto horns : DKpipi_horns) {
        m_shapes->AddHorns("4body_DKpipi_" + horns + "_shape", "DKpipi_a_" + horns, 
                "DKpipi_b_" + horns, "DKpipi_csi_" + horns, "shift", 
                "4body_DKpipi_sigma_" + horns, "DKpipi_ratio_" + horns,
                "DKpipi_frac_" + horns);
    }
    m_shapes->AddHorns("4body_DKpipi_2_shape", "DKpipi_a_2", "DKpipi_b_2", 
            "DKpipi_csi_2", "shift", "4body_DKpipi_sigma_2", "DKpipi_ratio_2",
            "DKpipi_frac_2");
    m_shapes->CombineShapes("4body_DKpipi", {
            {"4body_DKpipi_1a_shape", "DKpipi_coeff_1a"},
            {"4body_DKpipi_2_shape", "DKpipi_coeff_2"},
            {"4body_DKpipi_3_shape", "DKpipi_coeff_3"},
            {"4body_DKpipi_5a_shape", "DKpipi_coeff_5a"}});

    // Combinatorial shapes
    m_shapes->AddExponential("expo_Kpipipi", "slope_Kpipipi");
    m_shapes->AddExponential("expo_piKpipi", "slope_Kpipipi");
    m_shapes->AddExponential("expo_pipipipi", "slope_pipipipi");
}


// ==============================
// Make total shape for each mode
// ==============================
void TwoAndFourBodyBase::MakeModeShapes() {

    // Loop through modes 
    for (auto mode : m_modes) {

        // Map of shapes and yields
        std::map<std::string, std::string> shapes;

        // Get mode name without plus or minus
        std::string mode_short = mode;
        if (mode.find("_plus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_plus"));
        } else if (mode.find("_minus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_minus"));
        }

        // Shapes common to all modes
        if (mode_short == "Kpipipi" || mode_short == "piKpipi" ||
            mode_short == "pipipipi") {
            shapes.emplace("4body_signal", "n_signal_" + mode);
            shapes.emplace("4body_rho", "n_rho_" + mode);
        } else {
            shapes.emplace("signal", "n_signal_" + mode);
            shapes.emplace("rho", "n_rho_" + mode);
        }
        shapes.emplace("expo_" + mode_short, "n_expo_" + mode);
        shapes.emplace("low_" + mode_short, "n_low_" + mode);

        // Shapes for all except Kpi
        if (mode_short != "Kpi" && mode_short != "Kpipipi") {
            if (mode_short == "piKpipi" || mode_short == "pipipipi") {
                shapes.emplace("4body_Bs", "n_Bs_" + mode);
                shapes.emplace("4body_Bs_low", "n_Bs_low_" + mode);
            } else {
                shapes.emplace("Bs", "n_Bs_" + mode);
                shapes.emplace("Bs_low", "n_Bs_low_" + mode);
            }
        } else if (mode_short == "Kpi") {
            shapes.emplace("DKpipi", "n_DKpipi_" + mode);
        }

        // Make the shape
        m_shapes->CombineShapes(mode, shapes);
    }
}
