#include "ParameterReader.hpp"
#include "DataPdfMaker.hpp" 

typedef std::string str;

// ===========
// Constructor
// ===========
DataPdfMaker::DataPdfMaker(RooRealVar * x, RooCategory * cat, bool blind) :
    ShapeMakerBase("pdf", x, cat),
    m_blind(blind) {}

DataPdfMaker::DataPdfMaker(std::string name, RooRealVar * x, RooCategory * cat, 
        bool blind) : 
    ShapeMakerBase(name, x, cat),
    m_blind(blind) {}


// ======================================
// Set the signal yield of a mode to zero
// ======================================
void DataPdfMaker::SetZeroYield(std::string mode, bool set_zero) {

    // Add a flag to map
    bool valid = false;
    for (str valid_mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
        if (mode == valid_mode) {
            valid = true;
            break;
        }
    }
    if (!valid) {
        std::cout << "Warning: " << mode << " is not a valid mode for setting "
            "yield to zero! Ignoring." << std::endl;
        return;
    }
    m_zeroYields[mode] = set_zero;

    // Indicate that shape needs to be remade
    m_shapeMade = false;
}


// =======================
// Set constant parameters
// =======================
void DataPdfMaker::SetConstantParameters() {

    // Read in MC fit parameters
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Fit_monte_carlo/Results/");
    pr->ReadParameters("signal", "signal_Kpi.param");
    pr->ReadParameters("Bs", "signal_Bs.param");
    pr->ReadParameters("4body_signal", "signal_Kpipipi.param");
    pr->ReadParameters("rho", "rho_all_PIDcut.param");
    pr->ReadParameters("low", "lowMass.param");
    pr->ReadParameters("DKpipi", "DKpipi.param");
    pr->ReadParameters("gamma_pi", "../../Parameters/gamma_vs_pi.param");
    pr->ReadParameters("ratio", "../../Parameters/low_mass_Bs_ratio.param");
    pr->ReadParameters("prod_asym_B0", "../../Asymmetries/Production/Results/B0.txt");
    pr->ReadParameters("prod_asym_Bs", "../../Asymmetries/Production/Results/Bs.txt");

    // Difference between B0 and Bs masses
    m_pars->AddRealVar("delta_M", 87.26);

    // Crystal ball shape parameters (signal shapes, rho mis-ID)
    for (std::string cb : {"signal", "Bs", "rho", "4body_signal"}) {

        // Initial mean for B0 shapes (Bs mean fixed via delta M)
        if (cb != "Bs") m_pars->AddRealVar(cb + "_mean_before_shift", 
                pr->GetValue(cb, "mean"));

        // Fixed width for rho shape (all others float)
        if (cb == "rho") m_pars->AddRealVar(cb + "_sigma_L", 
                pr->GetValue(cb, "sigma_L"));

        // Other crystal ball shape parameters
        for (str cb_par : {"sigma_ratio", "alpha_L", "alpha_R", "n_L", "n_R", "frac"}) {
            m_pars->AddRealVar(cb + "_" + cb_par, pr->GetValue(cb, cb_par));
        }
    }

    // Low mass parameters
    // Kinematic endpoints
    for (str ep : {"a_gamma", "b_gamma", "a_pi", "b_pi"}) {
        m_pars->AddRealVar("low_" + ep, pr->GetValue("low", ep));
    }

    // Shared parameters
    m_pars->AddRealVar("low_ratio", pr->GetValue("low", "ratio"));
    m_pars->AddRealVar("low_frac", pr->GetValue("low", "frac"));
    m_pars->AddRealVar("shiftg", 0);

    // Other shape parameters
    for (str par : {"sigma", "csi"}) {
        for (str type : {"gamma_010", "gamma_101", "pi_010", "pi_101"}) {
            m_pars->AddRealVar("low_" + par + "_" + type, 
                    pr->GetValue("low", par + "_" + type));
            m_pars->AddRealVar("Bs_low_" + par + "_" + type, 
                    pr->GetValue("low", "Bs_" + par + "_" + type));
        }
    }

    // Efficiencies for coefficients for pi and gamma shapes
    for (str particle : {"gamma", "pi"}) {
        for (str hel : {"010", "101"}) {
            m_pars->AddRealVar("sel_eff_" + particle + "_" + hel, 
                    pr->GetValue("gamma_pi", "sel_eff_" + particle + "_" + hel));
        }
        m_pars->AddRealVar("acc_eff_" + particle, pr-> GetValue("gamma_pi", "acc_eff_" + particle));
        m_pars->AddRealVar("br_" + particle, pr-> GetValue("gamma_pi", "br_" + particle));
    }

    // Ratios between low mass Bs yields
    m_pars->AddRealVar("R_KK_vs_piK_Bs_low", pr->GetValue("ratio", "KK_vs_piK"));
    m_pars->AddRealVar("R_pipi_vs_piK_Bs_low", pr->GetValue("ratio", "pipi_vs_piK"));
    m_pars->AddRealVar("R_pipipipi_vs_piKpipi_Bs_low", pr->GetValue("ratio", "pipipipi_vs_piKpipi"));

    // DKpipi shape parameters
    std::vector<std::string> DKpipi_hillPars = {"a", "b", "csi", "sigma", "ratio", 
        "frac"};
    std::vector<std::string> DKpipi_gaussPars = {"mean", "sigma", "f"};
    for (auto par : DKpipi_hillPars) {
        m_pars->AddRealVar("DKpipi_" + par, pr->GetValue("DKpipi", par));
    }
    for (auto par : DKpipi_gaussPars) {
        m_pars->AddRealVar("DKpipi_" + par + "_gauss1", pr->GetValue("DKpipi", par + "_gauss1"));
        m_pars->AddRealVar("DKpipi_" + par + "_gauss2", pr->GetValue("DKpipi", par + "_gauss2"));
    }

    // Asymmetry corrections
    // Production asymmetry
    m_pars->AddRealVar("A_prod_B0", (pr->GetValue("prod_asym_B0", "2012_down")
                + pr->GetValue("prod_asym_B0", "2012_up"))/2);
    m_pars->AddRealVar("A_prod_Bs", (pr->GetValue("prod_asym_Bs", "2012_down")
                + pr->GetValue("prod_asym_Bs", "2012_up"))/2);

}


// ======================================
// Set parameters which are fixed to zero
// ======================================
void DataPdfMaker::SetZeroParameters() {

    // Zero asymmetries
    // Rho and combinatorial
    for (str shape : {"rho", "expo"}) {
        for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
            m_pars->AddRealVar("A_" + mode + "_" + shape + "_raw", 0);
        }
    }
    for (str mode : {"piK", "piKpipi"}) {
        m_pars->AddRealVar("A_" + mode + "_expo_raw", 0);
    }

    // Bs low mass
    for (str mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
        m_pars->AddRealVar("A_" + mode + "_Bs_low", 0);
        m_asyms_Bs.push_back("A_" + mode + "_Bs_low");
    }

    // DKpipi in favoured modes
    for (str mode : {"Kpi", "Kpipipi"}) {
        m_pars->AddRealVar("A_" + mode + "_DKpipi", 0);
        m_asyms_B0.push_back("A_" + mode + "_DKpipi");
    }

    // Rho ratio = 1 between ADS suppressed and favoured
    for (str mode : {"piK", "piKpipi"}) {
        for (str sign : {"plus", "minus"}) {
            m_pars->AddRealVar("R_" + sign + "_rho_" + mode, 1);
        }
    }
    m_pars->AddRealVar("R_piK_vs_Kpi_rho", 1);
    m_pars->AddRealVar("R_piKpipi_vs_Kpipipi_rho", 1);

}


// =======================
// Set floating parameters
// =======================
void DataPdfMaker::SetFloatingParameters() {    

    // Global shift
    m_pars->AddRealVar("shift", 0, -10, 10);

    // Signal widths
    // Read starting widths
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Fit_monte_carlo/Results/");
    pr->ReadParameters("signal", "signal_Kpi.param");
    pr->ReadParameters("Bs", "signal_Bs.param");
    pr->ReadParameters("4body_signal", "signal_Kpipipi.param");

    // Set signal widths to float around starting values
    for (str shape : {"signal", "Bs", "4body_signal"}) {
        double start = pr->GetValue(shape, "sigma_L");
        m_pars->AddRealVar(shape + "_sigma_L", start, start - 10, start + 10);
    }

    // Ratio between four body and two body widths
    m_pars->AddRealVar("four_vs_two_body_ratio", 1.06, 0.5, 2);

    // Helicity fraction for low mass shapes
    std::vector<std::string> frac_types = {"Kpi", "piK", "GLW", "Kpipipi", 
        "piKpipi", "pipipipi"};
    for (str mode : {"Kpi", "piK", "GLW", "Kpipipi", "piKpipi", "pipipipi"}) {

        // Overall fraction
        m_pars->AddRealVar("low_frac_010_" + mode, 0.5, 0, 1);

        // Different fractions for plus and minus
        if (mode != "Kpi" && mode != "Kpipipi") {
            m_pars->AddRealVar("low_frac_010_" + mode + "_plus", 0.5, 0, 1);
            m_pars->AddRealVar("low_frac_010_" + mode + "_minus", 0.5, 0, 1);
        }
    }
    m_pars->AddRealVar("Bs_low_frac_010", 0.5, 0, 1);

    // Slope of exponentials
    for (str slope : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        m_pars->AddRealVar("slope_" + slope, -0.005, -0.01, 0);
    }

    // Yield ratios
    std::string bl = m_blind ? "_blind" : "";
    std::vector<std::string> yields = {"signal" + bl, "low", "DKpipi"};
    for (str type : yields) {
        m_pars->AddRealVar("R_piK_vs_Kpi_" + type, 0.06, 0, 1);
        m_pars->AddRealVar("R_KK_vs_Kpi_" + type, 0.1, 0, 1);
        m_pars->AddRealVar("R_pipi_vs_Kpi_" + type, 0.03, 0, 1);
        m_pars->AddRealVar("R_piKpipi_vs_Kpipipi_" + type, 0.06, 0, 1);
        m_pars->AddRealVar("R_pipipipi_vs_Kpipipi_" + type, 0.03, 0, 1);
    }

    // Make unblind versions of signal yields
    if (m_blind) {
        m_pars->AddUnblindVar("R_piK_vs_Kpi_signal", "R_piK_vs_Kpi_signal_blind", 
                "blind_piK_ratio", 0.01);
        m_pars->AddUnblindVar("R_KK_vs_Kpi_signal", "R_KK_vs_Kpi_signal_blind", 
                "blind_KK_ratio", 0.1);
        m_pars->AddUnblindVar("R_pipi_vs_Kpi_signal", "R_pipi_vs_Kpi_signal_blind", 
                "blind_pipi_ratio", 0.05);
        m_pars->AddUnblindVar("R_piKpipi_vs_Kpipipi_signal", 
                "R_piKpipi_vs_Kpipipi_signal_blind", "blind_piKpipi_ratio", 0.01);
        m_pars->AddUnblindVar("R_pipipipi_vs_Kpipipi_signal", 
                "R_pipipipi_vs_Kpipipi_signal_blind", "blind_pipipipi_ratio", 0.05);
    }

    // Set any requested signal yields to zero
    SetZeroYields();

    // Bs ratios
    m_pars->AddRealVar("R_KK_vs_piK_Bs", 0.1, 0, 1);
    m_pars->AddRealVar("R_pipi_vs_piK_Bs", 0.03, 0, 1);
    m_pars->AddRealVar("R_pipipipi_vs_piKpipi_Bs", 0.03, 0, 1);
    
    // Background vs. signal ratios
    m_pars->AddRealVar("BF_R_low_vs_signal", 1.5, 1, 2);
    m_pars->AddRealVar("BF_R_DKpipi_vs_signal", 0.3, 0, 1);
    m_pars->AddRealVar("BF_R_Bs_low_vs_Bs", 1.1, 0.9, 2);

    // Floating yields
    for (str ext : {"", "pipi"}) {
        // Favoured mode yields
        double max_fav = GetMaxYield("Kpi" + ext);
        m_pars->AddRealVar("n_signal_Kpi" + ext, max_fav/3, 0, max_fav);
        m_pars->AddRealVar("n_rho_Kpi" + ext, max_fav/100, 0, max_fav/20);

        // Suppressed mode yields
        double max_sup = GetMaxYield("piK" + ext);
        m_pars->AddRealVar("n_Bs_piK" + ext, max_sup/3, 0, max_sup);
    }   

    // Asymmetries
    // Non-blind signal asymmetries
    for (str mode : {"Kpi", "Kpipipi"}) {
        m_pars->AddRealVar("A_" + mode + "_signal", 0, -1, 1);
        m_asyms_B0.push_back("A_" + mode + "_signal");
    }

    // (Blind) signal asymmetries
    for (str mode : {"KK", "pipi", "pipipipi"}) {
        m_pars->AddRealVar("A_" + mode + "_signal" + bl, 0, -1, 1);
        if (m_blind) {
            m_pars->AddUnblindVar("A_" + mode + "_signal", 
                    "A_" + mode + "_signal" + bl, "blind_" + mode + "_asym", 0.01);
        }
        m_asyms_B0.push_back("A_" + mode + "_signal");
    }

    // Bs asymmetries
    for (str asym : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
        m_pars->AddRealVar("A_" + asym + "_Bs", 0, -1, 1);
        m_asyms_Bs.push_back("A_" + asym + "_Bs");
    }

    // Low mass asymmetries
    for (str asym : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        m_pars->AddRealVar("A_" + asym + "_low", 0, -1, 1);
        m_asyms_B0.push_back("A_" + asym + "_low");
        if (asym != "Kpi" && asym != "Kpipipi") {
            m_pars->AddRealVar("A_" + asym + "_DKpipi", 0, -1, 1);
            m_asyms_B0.push_back("A_" + asym + "_DKpipi");
        }
    }

    // ADS mode ratios by flavour
    for (str mode : {"piK", "piKpipi"}) {
        for (str sign : {"plus", "minus"}) {
            for (str type : yields) {
                m_pars->AddRealVar("R_" + sign + "_" + type + "_" + mode, 0.06, 0, 1);
            }
            if (m_blind) {
                m_pars->AddUnblindVar("R_" + sign + "_signal_" + mode,
                        "R_" + sign + "_signal_blind_" + mode, 
                        "blind_R_" + sign + "_" + mode, 0.01);
            }
        }
    }

    // Exponential yields
    for (str mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi", "pipipipi"}) {
        m_pars->AddRealVar("n_expo_" + mode, GetMaxYield(mode)/4, 0, 
                GetMaxYield(mode));
    }
}


// ==========================================
// Set parameters with dependencies on others
// ==========================================
void DataPdfMaker::SetDependentParameters() {

    // ================
    // Shape parameters
    // ================

    // Make sure all zero parameters have been set
    SetZeroParameters();

    // Shifted parameters
    for (str to_shift : {"signal", "rho", "4body_signal"}) {
        m_pars->AddSummedVar(to_shift + "_mean", to_shift + "_mean_before_shift", "shift");
    }
    m_pars->AddSummedVar("Bs_mean", "signal_mean", "delta_M");
    m_pars->AddSummedVar("4body_Bs_mean", "4body_signal_mean", "delta_M");

    // Widths from ratios
    for (str shape : {"signal", "Bs", "rho", "4body_signal"}) {
        m_pars->AddProductVar(shape + "_sigma_R", shape + "_sigma_L", shape + "_sigma_ratio");
    }

    // Shift Bs low mass endpoints
    for (str particle : {"gamma", "pi"}) {
        m_pars->AddSummedVar("Bs_low_a_" + particle, "low_a_" + particle, "delta_M");
        m_pars->AddSummedVar("Bs_low_b_" + particle, "low_b_" + particle, "delta_M");
    }

    // Coefficients of pi and gamma shapes
    for (str hel : {"010", "101"}) {
        m_pars->AddFormulaVar("G_" + hel, "@0 * @1 * @2", 
                ParameterList("br_gamma", "acc_eff_gamma", "sel_eff_gamma_" + hel));
        m_pars->AddFormulaVar("P_" + hel, "@0 * @1 * @2", 
                ParameterList("br_pi", "acc_eff_pi", "sel_eff_pi_" + hel));
        m_pars->AddFormulaVar("coeff_gamma_" + hel, "@0/(@0 + @1)", 
                ParameterList("G_" + hel, "P_" + hel));
        m_pars->AddFormulaVar("coeff_pi_" + hel, "1 - @0", 
                ParameterList("coeff_gamma_" + hel));
    }

    // Shared helicity fractions for CP modes
    for (str mode : {"KK", "pipi"}) {
        for (str sign : {"", "_plus", "_minus"}) {
            m_pars->AddShared("low_frac_010_" + mode + sign, "low_frac_010_GLW" + sign);
        }
    }

    // Same helicity fraction for plus and minus in favoured modes
    for (str mode : {"Kpi", "Kpipipi"}) {
        for (str sign : {"plus", "minus"}) {
            m_pars->AddShared("low_frac_010_" + mode + "_" + sign, 
                    "low_frac_010_" + mode);
        }
    }

    // Four body widths scaled by ratio wrt. two body
    std::vector<std::string> widths_to_scale = {"Bs_sigma_L", "Bs_sigma_R", 
        "low_sigma_gamma_010", "low_sigma_gamma_101", "low_sigma_pi_010", 
        "low_sigma_pi_101", "Bs_low_sigma_gamma_010", "Bs_low_sigma_gamma_101", 
        "Bs_low_sigma_pi_010", "Bs_low_sigma_pi_101", "DKpipi_sigma", 
        "DKpipi_sigma_gauss1", "DKpipi_sigma_gauss2", "rho_sigma_L", 
        "rho_sigma_R"};
    for (str width : widths_to_scale) {
        m_pars->AddProductVar("4body_" + width, width, "four_vs_two_body_ratio");
    }

    // Shared slopes
    for (str ext : {"", "pipi"}) {
        m_pars->AddShared("slope_piK" + ext, "slope_Kpi" + ext);
    }


    // ==================
    // Adjust asymmetries
    // ==================
    // Make raw asymmetry variables
    for (auto asym : m_asyms_B0) {
        m_pars->AddFormulaVar(asym + "_raw", "@0 + @1", ParameterList(asym, "A_prod_B0"));
    }
    for (auto asym : m_asyms_Bs) {
        m_pars->AddFormulaVar(asym + "_raw", "@0 + @1", ParameterList(asym, "A_prod_Bs"));
    }
    m_pars->Print();


    // ======
    // Yields
    // ======
    // Shared ratios
    m_pars->AddShared("R_KK_vs_Kpi_rho", "R_KK_vs_piK_Bs_low");
    m_pars->AddShared("R_pipi_vs_Kpi_rho", "R_pipi_vs_piK_Bs_low");
    m_pars->AddShared("R_pipipipi_vs_Kpipipi_rho", "R_pipipipi_vs_piKpipi_Bs_low");

    // Background yields from signal
    for (str mode : {"Kpi", "Kpipipi"}) {
        for (str bg : {"low", "DKpipi"}) {
            m_pars->AddProductVar("n_" + bg + "_" + mode, "n_signal_" + mode,
                    "BF_R_" + bg + "_vs_signal");
        }
    }
    for (str mode : {"piK", "piKpipi"}) {
        m_pars->AddProductVar("n_Bs_low_" + mode, "n_Bs_" + mode,
                "BF_R_Bs_low_vs_Bs");
    }

    // Bd yields
    for (str shape : {"signal", "low", "rho", "DKpipi"}) {
        // Two body
        for (str mode : {"piK", "pipi", "KK"}) {
            m_pars->AddProductVar("n_" + shape + "_" + mode, 
                    "n_" + shape + "_Kpi",
                    "R_" + mode + "_vs_Kpi_" + shape);
        }
        // Four body
        for (str mode : {"piKpipi", "pipipipi"}) {
            m_pars->AddProductVar("n_" + shape + "_" + mode, 
                    "n_" + shape + "_Kpipipi",
                    "R_" + mode + "_vs_Kpipipi_" + shape);
        }
    }

    // Bs yields: two body
    for (str shape : {"Bs", "Bs_low"}) {
        // Two body
        for (str mode : {"KK", "pipi"}) {
            m_pars->AddProductVar("n_" + shape + "_" + mode,
                    "n_" + shape + "_piK",
                    "R_" + mode + "_vs_piK_" + shape);
        }
        // Four body
        m_pars->AddProductVar("n_" + shape + "_pipipipi",
                "n_" + shape + "_piKpipi",
                "R_pipipipi_vs_piKpipi_" + shape);
    }

    // Split signal/low mass yields using asymmetries
    if (IsSplit()) {

        // Bd
        for (str shape : {"signal", "low", "rho", "DKpipi", "expo"}) {
    
            // Split with asymmetries
            for (str mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "pipipipi", "piKpipi"}) {
                if ((mode == "piK" || mode == "piKpipi") && shape != "expo") continue;
                m_pars->AddFormulaVar("a_" + mode + "_" + shape,
                        "(1 + @0)/(1 - @0)", ParameterList("A_" + mode + "_" + shape + "_raw"));
                m_pars->AddFormulaVar("n_" + shape + "_" + mode + "_plus", "@0/(1 + @1)",
                        ParameterList("n_" + shape + "_" + mode, "a_" + mode + "_" + shape));
                m_pars->AddFormulaVar("n_" + shape + "_" + mode + "_minus", "@0/(1 + 1/@1)",
                        ParameterList("n_" + shape + "_" + mode, "a_" + mode + "_" + shape));
            }
        }

        // Split suppressed ADS yields using ratios
        for (str shape : {"signal", "low", "rho", "DKpipi"}) {
            for (str extra : {"", "pipi"}) {
                for (str sign :{"plus", "minus"}) {
                    m_pars->AddProductVar("n_" + shape + "_piK" + extra + "_" + sign,
                            "n_" + shape + "_Kpi" + extra + "_" + sign,
                            "R_" + sign + "_" + shape + "_piK" + extra);
                }
            }
        }

        // Bs
        for (str shape : {"Bs", "Bs_low"}) {
            for (str mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
                m_pars->AddFormulaVar("a_" + mode + "_" + shape,
                        "(1 + @0)/(1 - @0)", ParameterList("A_" + mode + "_" + shape + "_raw"));
                m_pars->AddFormulaVar("n_" + shape + "_" + mode + "_plus", "@0/(1 + 1/@1)",
                        ParameterList("n_" + shape + "_" + mode, "a_" + mode + "_" + shape));
                m_pars->AddFormulaVar("n_" + shape + "_" + mode + "_minus", "@0/(1 + @1)",
                        ParameterList("n_" + shape + "_" + mode, "a_" + mode + "_" + shape));
            }
        }
    }
}

// =========================
// Make each component shape
// =========================
void DataPdfMaker::MakeComponentShapes() {

    // Double crystal ball shapes with free width
    for (str cb : {"signal", "rho", "Bs", "4body_signal"}) {
        for (str side : {"_L", "_R"}) {
            m_shapes->AddCrystalBall(cb + "_CB" + side, cb + "_mean", 
                    cb + "_sigma" + side, cb + "_alpha" + side, 
                    cb + "_n" + side);
        }
        m_shapes->CombineShapes(cb, cb + "_CB_L", cb + "_CB_R", cb + "_frac");
    }

    // Four body Bs/rho shapes
    for (str cb : {"Bs", "rho"}) {
        for (str side : {"_L", "_R"}) {
            m_shapes->AddCrystalBall("4body_" + cb + "_CB" + side, cb + "_mean", 
                    "4body_" + cb + "_sigma" + side, cb + "_alpha" + side, 
                    cb + "_n" + side);
        }
        m_shapes->CombineShapes("4body_" + cb, "4body_" + cb + "_CB_L", 
                "4body_" + cb + "_CB_R", cb + "_frac");
    }

    // Shapes with different width for two and four body
    for (str bod : {"", "4body_"}) {

        // Low mass shapes
        for (str pre : {"", "Bs_"}) {

            // Make each shape
            m_shapes->AddHill(bod + pre + "gamma_010_shape", pre + "low_a_gamma", 
                    pre + "low_b_gamma", pre + "low_csi_gamma_010", "shift", 
                    bod + pre + "low_sigma_gamma_010", "low_ratio", "low_frac");
            m_shapes->AddLittleHorns(bod + pre + "gamma_101_shape", pre + "low_a_gamma", 
                    pre + "low_b_gamma", pre + "low_csi_gamma_101", "shift", 
                    pre + "low_sigma_gamma_101", "low_ratio", "low_frac", "shiftg");
            m_shapes->AddHorns(bod + pre + "pi_010_shape", pre + "low_a_pi", 
                    pre + "low_b_pi", pre + "low_csi_pi_010", "shift", 
                    pre + "low_sigma_pi_010", "low_ratio", "low_frac");
            m_shapes->AddHill(bod + pre + "pi_101_shape", pre + "low_a_pi", 
                    pre + "low_b_pi", pre + "low_csi_pi_101", "shift", 
                    pre + "low_sigma_pi_101", "low_ratio", "low_frac");

            // Combine into helicity shapes
            m_shapes->CombineShapes(bod + pre + "low_010_shape", {
                    {bod + pre + "gamma_010_shape", "coeff_gamma_010"},
                    {bod + pre + "pi_010_shape", "coeff_pi_010"}});
            m_shapes->CombineShapes(bod + pre + "low_101_shape", {
                    {bod + pre + "gamma_101_shape", "coeff_gamma_101"},
                    {bod + pre + "pi_101_shape", "coeff_pi_101"}});
        }

        // Make overall Bs low mass shapes
        m_shapes->CombineShapes(bod + "Bs_low", bod + "Bs_low_010_shape",
                bod + "Bs_low_101_shape", "Bs_low_frac_010");

        // DKpipi shape
        m_shapes->AddHill(bod + "DKpipi_hill", "DKpipi_a", "DKpipi_b",
                "DKpipi_csi", "shift", bod + "DKpipi_sigma", "DKpipi_ratio",
                "DKpipi_frac");
        m_shapes->AddGaussian(bod + "DKpipi_gauss1", "DKpipi_mean_gauss1",
                bod + "DKpipi_sigma_gauss1");
        m_shapes->AddGaussian(bod + "DKpipi_gauss2", "DKpipi_mean_gauss2",
                bod + "DKpipi_sigma_gauss2");
        m_shapes->CombineShapes(bod + "DKpipi", {
                {bod + "DKpipi_gauss1", "DKpipi_f_gauss1"},
                {bod + "DKpipi_gauss2", "DKpipi_f_gauss2"},
                {bod + "DKpipi_hill", ""}});
    }

    // Combine helicity components of low mass shape
    for (str sign : {"", "_plus", "_minus"}) {
        for (str mode : {"Kpi", "piK", "KK", "pipi"}) { 
            m_shapes->CombineShapes("low_" + mode + sign, "low_010_shape", 
                    "low_101_shape", "low_frac_010_" + mode + sign);
        }
        for (str mode : {"Kpipipi", "piKpipi", "pipipipi"}) { 
            m_shapes->CombineShapes("low_" + mode + sign, "4body_low_010_shape", 
                    "4body_low_101_shape", "low_frac_010_" + mode + sign);
        }
    }

    // Combine Bs low mass shape

    // Combinatorial shapes
    for (str mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi", "pipipipi"}) {
        m_shapes->AddExponential("expo_" + mode, "slope_" + mode);
    }
}


// ==============================
// Make total shape for each mode
// ==============================
void DataPdfMaker::MakeModeShapes() {

    // Loop through modes 
    for (str mode : m_modes) {

        // Map of shapes and yields
        std::map<std::string, std::string> shapes;

        // Get mode name without plus or minus
        std::string mode_short = mode;
        if (mode.find("_plus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_plus"));
        } else if (mode.find("_minus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_minus"));
        }

        // Determine shape type
        bool is_four_body = (mode_short == "Kpipipi" || 
                mode_short == "piKpipi" || mode_short == "pipipipi");
        bool is_favoured = (mode_short == "Kpi" || mode_short == "Kpipipi");
        std::string type = is_four_body ? "4body_" : "";

        // Shapes common to all modes
        shapes.emplace(type + "signal", "n_signal_" + mode);
        shapes.emplace(type + "rho", "n_rho_" + mode);
        shapes.emplace("expo_" + mode_short, "n_expo_" + mode);
        shapes.emplace("low_" + mode, "n_low_" + mode);

        // Shapes for suppressed modes only
        if (!is_favoured) {
            shapes.emplace(type + "Bs", "n_Bs_" + mode);
            shapes.emplace(type + "Bs_low", "n_Bs_low_" + mode);
        }

        // DKpipi shape
        shapes.emplace(type + "DKpipi", "n_DKpipi_" + mode);

        // Make the shape
        m_shapes->CombineShapes(mode, shapes);
    }
}


// ==================
// Set yields to zero
// ==================
void DataPdfMaker::SetZeroYields() {
    m_pars->SetWarnings(false);
    for (auto zero : m_zeroYields) {
        if (zero.second) {
            std::string comparison_mode = "Kpi";
            if (zero.first == "piKpipi" || zero.first == "pipipipi") {
                comparison_mode = "Kpipipi";
            }
            m_pars->AddRealVar("R_" + zero.first + "_vs_" + comparison_mode + 
                    "_signal", 0);
        }
    }
    m_pars->SetWarnings(true);
}


// =====================================
// Check whether shapes are split or not
// =====================================
bool DataPdfMaker::IsSplit() {

    // Check mode list for plus and minus labels
    bool split = true;
    for (str mode : m_modes) {
        if (mode.find("_plus") == std::string::npos && 
                mode.find("_minus") == std::string::npos) {
            split = false;
        }
    }
    return split;
}


// =============================================
// Get integral of signal withing B0 mass region
// =============================================
double DataPdfMaker::GetSignalIntegral(std::string mode) {

    // Name of signal shape
    std::string signal_name = "signal";
    if (mode.find("Kpipipi") != std::string::npos) signal_name = "4body_signal";
    else if (mode.find("piKpipi") != std::string::npos) signal_name = "4body_signal";
    else if (mode.find("pipipipi") != std::string::npos) signal_name = "4body_signal";

    // Return integral
    return GetComponentIntegral(signal_name, "n_signal_" + mode, m_Bmass - m_Brange,
            m_Bmass + m_Brange);
}


// ===========================================================
// Get integral of exponential component within B0 mass region
// ===========================================================
double DataPdfMaker::GetCombinatorialIntegral(std::string mode) {
    std::string mode_short = mode;
    for (std::string sign : {"_plus", "_minus"}) {
        mode_short = mode_short.substr(0, mode_short.find(sign));
    }
    return GetComponentIntegral("expo_" + mode_short, "n_expo_" + mode,
            m_Bmass - m_Brange, m_Bmass + m_Brange);
}

// ==========================================================
// Get integral of all non-signal components in signal region
// ==========================================================
double DataPdfMaker::GetBackgroundIntegral(std::string mode) {
    double int_signal = GetSignalIntegral(mode);
    double int_total = GetFitIntegral(mode, m_Bmass - m_Brange, m_Bmass + m_Brange);
    return int_total - int_signal;
}


// ===============
// Save histograms
// ===============
void DataPdfMaker::SaveHistograms(std::string filename) {
    ShapeMakerBase::SaveHistograms(filename, m_blind);
}


// =========================
// Save histograms with data
// =========================
void DataPdfMaker::SaveHistograms(std::string filename, RooDataHist * data) {
    ShapeMakerBase::SaveHistograms(filename, data, m_blind);
}
