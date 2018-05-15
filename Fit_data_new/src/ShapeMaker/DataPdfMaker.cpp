#include "ParameterReader.hpp"
#include "DataPdfMaker.hpp" 

typedef std::string str;

// ===========
// Constructor
// ===========
DataPdfMaker::DataPdfMaker(RooRealVar * x, RooCategory * cat, bool blind) :
    ShapeMakerBase("pdf", x, cat),
    m_blind(blind),
    m_asyms_made(false),
    m_ratios_made(false) {}

DataPdfMaker::DataPdfMaker(std::string name, RooRealVar * x, RooCategory * cat, 
        bool blind) : 
    ShapeMakerBase(name, x, cat),
    m_blind(blind) {}


// ==========================
// Make components of the fit
// ==========================
void DataPdfMaker::MakeComponents() {
    MakeSharedParameters();
    MakeSignalShape();
    MakeLowMassShape();
    MakeRhoShape();
    MakeDKpipiShape();
    MakeCombiShape();
}


// ===========================================
// Make parameters used by multiple components
// ===========================================
void DataPdfMaker::MakeSharedParameters() {

    // Global shift
    m_pars->AddRealVar("shift", 0, -10, 10);

    // Smear factor for 4-body shapes
    m_pars->AddRealVar("four_vs_two_body_ratio", 1.06, 0.5, 2);

    // Asymmetry corrections
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/"
            "B02DKstar/");
    pr->ReadParameters("A_prod", "Asymmetries/Production/Results/"
            "production_asymmetry.param");
    pr->ReadParameters("A_det", "Asymmetries/Detection/Results/A_Kpi.param");
    m_pars->AddRealVar("A_prod_B0", pr->GetValue("A_prod", "B0"));
    m_pars->AddRealVar("A_prod_Bs", pr->GetValue("A_prod", "Bs"));
    m_pars->AddRealVar("A_det", pr->GetValue("A_det", "A_Kpi"));

    // Ratio corrections
    // Create for each mode
    for (str eff : {"selection", "acceptance", "PID"}) {
        pr->ReadParameters(eff, "Efficiencies/Values/" + eff + "_efficiency.param");
        for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
            m_pars->AddRealVar(eff + "_efficiency_" + mode, pr->GetValue(eff, mode));
        }
    }

    // Calculate overall efficiency factor for each mode
    for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        m_pars->AddFormulaVar("efficiency_" + mode, "@0 * @1 * @2",
                ParameterList("selection_efficiency_" + mode,
                    "acceptance_efficiency_" + mode, "PID_efficiency_" + mode));
    }

    // PID efficiencies split by B0/B0bar
    for (str parent : {"B0", "B0bar"}) {
        pr->ReadParameters("PID_" + parent, "Efficiencies/Values/PID_efficiency_"
                + parent + ".param");
        for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
            m_pars->AddRealVar("PID_" + parent + "_" + mode,
                    pr->GetValue("PID_" + parent, mode));
        }
    }

    // Read in branching fractions
    pr->ReadParameters("BF", "Parameters/branching_fractions.param");
    for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        m_pars->AddRealVar("BF_" + mode, pr->GetValue("BF", mode));
    }

    // Get correction factor to apply to B0bar w.r.t. B0
    // Production asymmetry factor
    for (str particle : {"B0", "Bs"}) {
        m_pars->AddFormulaVar("a_prod_" + particle, "(1 - @0)/(1 + @0)",
                ParameterList("A_prod_" + particle));
    }

    // Set detection asymmetry for each mode
    for (str mode : {"Kpi", "Kpipipi"}) {
        m_pars->AddFormulaVar("A_det_" + mode, "2 * @0", ParameterList("A_det"));
    }
    for (str mode : {"KK", "pipi", "pipipipi"}) {
        m_pars->AddFormulaVar("A_det_" + mode, "1 * @0", ParameterList("A_det"));
        m_pars->AddFormulaVar("A_det_" + mode + "_s", "-1 * @0", ParameterList("A_det"));
    }
    for (str mode : {"piK", "piKpipi"}) {
        m_pars->AddFormulaVar("a_det_" + mode, "(1  - @0)/(1 + @0)",
                ParameterList("A_det"));
        m_pars->AddFormulaVar("A_det_" + mode + "_s", "0 * @0", ParameterList("A_det"));
    }

    // Calculate detection asymmetry factor
    for (str mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi", "pipipipi"}) {
        if (mode != "piK" && mode != "piKpipi") {
            m_pars->AddFormulaVar("a_det_" + mode, "(1 - @0)/(1 + @0)",
                    ParameterList("A_det_" + mode));
        }
        if (mode != "Kpi" && mode != "Kpipipi") {
            m_pars->AddFormulaVar("a_det_" + mode + "_s", "(1 - @0)/(1 + @0)",
                    ParameterList("A_det_" + mode + "_s"));
        }
    }

    // Calculate PID efficiency factors
    for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        m_pars->AddFormulaVar("a_PID_" + mode, "@0/@1", 
                ParameterList("PID_B0_" + mode, "PID_B0bar_" + mode));
    }

    // Calculate total correction factor for each mode
    for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        m_pars->AddFormulaVar("a_corr_" + mode, "@0 * @1 * @2",
                ParameterList("a_PID_" + mode, "a_det_" + mode, "a_prod_B0"));
    }
    for (str mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
        std::string pid_mode = mode;
        if (mode == "piK") pid_mode = "Kpi";
        if (mode == "piKpipi") pid_mode = "Kpipipi";
        m_pars->AddFormulaVar("a_corr_" + mode + "_s", "@0 * @1 * @2",
                ParameterList("a_PID_" + pid_mode, "a_det_" + mode + "_s", "a_prod_Bs"));
    }

    // Calculate ratio correction factor for each mode
    for (str mode : {"KK", "pipi", "pipipipi"}) {
        std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
        m_pars->AddFormulaVar("R_corr_" + mode, "(@0 * @1)/(@2 * @3)",
                ParameterList("efficiency_" + fav, "BF_" + fav,
                    "efficiency_" + mode, "BF_" + mode));
    }

    // Get d vs. s parameters
    pr->ReadParameters("f", "Parameters/hadronization_fraction.param");
    pr->ReadParameters("lifetime", "Parameters/lifetimes.param");
    m_pars->AddRealVar("fs_fd", pr->GetValue("f", "fs_fd"));
    m_pars->AddRealVar("tau_d", pr->GetValue("lifetime", "B0"));
    m_pars->AddRealVar("tau_s", pr->GetValue("lifetime", "Bs"));
    m_pars->AddFormulaVar("R_corr_ds", "@0 * @1/@2", ParameterList("fs_fd",
                "tau_s","tau_d"));
}


// ==================
// Make signal shapes
// ==================
void DataPdfMaker::MakeSignalShape() {

    // Parameter reader
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Fit_monte_carlo/Results/");

    // Loop through signal shapes
    for (str shape : {"Kpi", "Bs", "Kpipipi"}) {

        // Get shape name
        std::string name = shape;
        if (shape == "Kpi") name = "signal";
        else if (shape == "Kpipipi") name = "4body_signal";

        // Read in constant parameters
        pr->ReadParameters(name, "signal_" + shape + ".param");
        for (str par : {"alpha_L", "alpha_R", "frac", "n_L", "n_R",
                "sigma_ratio"}) {
            m_pars->AddRealVar(name + "_" + par, pr->GetValue(name, par));
        }

        // Apply global shift to mean
        m_pars->AddRealVar(name + "_mean_preshift", pr->GetValue(name, "mean"));
        m_pars->AddSummedVar(name + "_mean", name + "_mean_preshift", "shift");

        // Float widths
        double sigma_start = pr->GetValue(name, "sigma_L");
        m_pars->AddRealVar(name + "_sigma_L", sigma_start, sigma_start - 10,
                sigma_start + 10);

        // Calculate sigma_R
        m_pars->AddProductVar(name + "_sigma_R", name + "_sigma_L",
                name + "_sigma_ratio");

        // Make shape
        for (str side : {"_L", "_R"}) {
            m_shapes->AddCrystalBall(name + "_CB" + side, name + "_mean",
                    name + "_sigma" + side, name + "_alpha" + side, 
                    name + "_n" + side);
        }
        m_shapes->CombineShapes(name, name + "_CB_L", name + "_CB_R", name + "_frac");

    }

    // Make 4-body Bs shape (adjusted width)
    for (str side : {"_L", "_R"}) {
        m_pars->AddProductVar("4body_Bs_sigma" + side, "Bs_sigma" + side, 
                "four_vs_two_body_ratio");
        m_shapes->AddCrystalBall("4body_Bs_CB" + side, "Bs_mean", 
                "4body_Bs_sigma" + side, "Bs_alpha" + side, "Bs_n" + side);
    }
    m_shapes->CombineShapes("4body_Bs", "4body_Bs_CB_L", "4body_Bs_CB_R", "Bs_frac");

    // Make favoured yields
    for (str fav : {"Kpi", "Kpipipi"}) {

        // Floating asymmetry and total yield
        double max_fav = GetMaxYield(fav);
        m_pars->AddRealVar("A_signal_" + fav, 0, -1, 1);
        m_pars->AddRealVar("N_signal_" + fav, max_fav/3, 0, max_fav);

        // Calculate raw yields from these
        m_pars->AddFormulaVar("N_signal_" + fav + "_plus", "@0 * (1 - @1)/2",
                ParameterList("N_signal_" + fav, "A_signal_" + fav));
        m_pars->AddFormulaVar("N_signal_" + fav + "_minus", "@0 * (1 - @1)/(2 * @2)",
                ParameterList("N_signal_" + fav, "A_signal_" + fav, 
                    "a_corr_" + fav));
    }

    // Make KK/pipi/pipipipi yields
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Make blind ratio and asymmetry
        std::string type = m_blind ? "_blind" : "";
        m_pars->AddRealVar("R_signal_" + mode + type, 1, 0.5, 1.5);
        m_pars->AddRealVar("A_signal_" + mode + type, 0, -1, 1);
        if (m_blind) {
            m_pars->AddUnblindVar("A_signal_" + mode, "A_signal_" + mode + "_blind",
                    "blind_" + mode + "_asym", 0.005);
            m_pars->AddUnblindVar("R_signal_" + mode, "R_signal_" + mode + "_blind",
                    "blind_" + mode + "_ratio", 0.005);
        }

        // Calculate raw yields
        std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
        m_pars->AddFormulaVar("N_signal_" + mode, "@0 * @1 / @2",
                ParameterList("N_signal_" + fav, "R_signal_" + mode,
                    "R_corr_" + mode));
        m_pars->AddFormulaVar("N_signal_" + mode + "_plus", 
                "@0 * @1 * (1 - @2) / (2 * @3)", ParameterList("N_signal_" + fav,
                    "R_signal_" + mode, "A_signal_" + mode, "R_corr_" + mode));
        m_pars->AddFormulaVar("N_signal_" + mode + "_minus", 
                "@0 * @1 * (1 + @2) / (2 * @3 * @4)", ParameterList("N_signal_" + fav,
                    "R_signal_" + mode, "A_signal_" + mode, "R_corr_" + mode,
                    "a_corr_" + mode));
    }

    // Make piK/piKpipi yields
    for (str mode : {"piK", "piKpipi"}) {

        // Yield ratios
        std::string fav = (mode == "piK") ? "Kpi" : "Kpipipi";
        std::string type = m_blind ? "_blind" : "";
        for (str sign : {"", "_plus", "_minus"}) {
            m_pars->AddRealVar("R_signal_" + mode + sign + type, 0.06, 0, 1);
            if (m_blind) {
                m_pars->AddUnblindVar("R_signal_" + mode + sign, 
                        "R_signal_" + mode + sign + "_blind", 
                        "blind_" + mode + "_ratio" + sign, 0.01);
            }
        }

        // Get yields from ratios
        m_pars->AddProductVar("N_signal_" + mode, "R_signal_" + mode, 
                "N_signal_" + fav);
        m_pars->AddFormulaVar("N_signal_" + mode + "_plus", "@0 * @1 / @2",
                ParameterList("N_signal_" + fav + "_plus", "R_signal_" + mode + 
                    "_plus", "a_det_" + mode));
        m_pars->AddFormulaVar("N_signal_" + mode + "_minus", "@0 * @1 / @2",
                ParameterList("N_signal_" + fav + "_minus", "R_signal_" + mode + 
                    "_minus", "a_det_" + mode));
    }

    // Bs yields
    // Suppressed mode yields and asymmetry
    for (str sup : {"piK", "piKpipi"}) {
            
        // Floating asymmetry and total yield
        double max_sup = GetMaxYield(sup);
        m_pars->AddRealVar("A_Bs_" + sup, 0, -1, 1);
        m_pars->AddRealVar("N_Bs_" + sup, max_sup/3, 0, max_sup);

        // Calculate raw yields from these
        m_pars->AddFormulaVar("N_Bs_" + sup + "_minus", "@0 * (1 - @1)/2",
                ParameterList("N_Bs_" + sup, "A_Bs_" + sup));
        m_pars->AddFormulaVar("N_Bs_" + sup + "_plus", 
                "@0 * (1 - @1)/(2 * @2)", ParameterList("N_Bs_" + sup, 
                    "A_Bs_" + sup, "a_corr_" + sup + "_s"));
    }

    // KK, pipi, pipipipi ds ratios
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Make ratio and asymmetry
        m_pars->AddRealVar("A_Bs_" + mode, 0, -1, 1);
        std::string type = m_blind ? "_blind" : "";
        m_pars->AddRealVar("R_ds_" + mode + type, 0.1, 0, 1);
        if (m_blind) {
            m_pars->AddUnblindVar("R_ds_" + mode, "R_ds_" + mode + "_blind",
                    "blind_ds_ratio_" + mode, 0.01);
        }

        // Calculate raw Bs yields from these
        m_pars->AddFormulaVar("N_Bs_" + mode, "@0 * @1 / @2", 
                ParameterList("N_signal_" + mode, "R_corr_ds", "R_ds_" + mode));
        m_pars->AddFormulaVar("N_Bs_" + mode + "_minus", "@0 * (1 + @1)/(2 * @2)",
                ParameterList("N_Bs_" + mode, "A_Bs_" + mode, "a_corr_" + mode + 
                    "_s"));
        m_pars->AddFormulaVar("N_Bs_" + mode + "_plus", "@0 * (1 - @1)/2",
                ParameterList("N_Bs_" + mode, "A_Bs_" + mode)); 
    }
}


// ==============================
// Make Bs and B0 low mass shapes
// ==============================
void DataPdfMaker::MakeLowMassShape() {

    // Get shape parameters
    ParameterReader * pr = new ParameterReader("../Fit_monte_carlo/Results/");
    m_pars->AddRealVar("csi_adjustment", 1, 0.5, 1.5);
    for (str parent : {"", "Bs_"}) {
        for (str particle : {"pi_", "gamma_"}) {
            for (str hel : {"010", "101"}) {

                // Read from file
                std::string name = parent + particle + hel;
                pr->ReadParameters(name, "lowMass_" + name + ".param");

                // Read in parameters
                for (str par : {"a", "b", "csi", "sigma", "frac", "ratio"}) {
                    m_pars->AddRealVar(name + "_" + par, pr->GetValue(name, par));
                }

                // Make adjusted 4-body width
                m_pars->AddProductVar("4body_" + name + "_sigma", name + "_sigma",
                        "four_vs_two_body_ratio");

                // // Make adjusted csi
                // m_pars->AddRealVar(name + "_csi_presmear",
                        // pr->GetValue(name, "csi"));
                // m_pars->AddProductVar(name + "_csi", name + "_csi_presmear",
                        // "csi_adjustment");
            }
        }
    }

    // Make shiftg (fixed to zero) for LittleHorns
    m_pars->AddRealVar("shiftg", 0);

    // Get selection efficiencies 
    for (str eff : {"selection", "acceptance"}) {
        pr->ReadParameters(eff, "../../Efficiencies/Values/" + eff + "_lowMass.param");
        for (str mode : {"gamma_010", "gamma_101", "pi_010", "pi_101"}) {
            m_pars->AddRealVar(mode + "_" + eff, pr->GetValue(eff, mode));
        }
    }

    // Get branching fractions
    pr->ReadParameters("BF", "../../Parameters/branching_fractions_lowMass.param");
    m_pars->AddRealVar("BF_gamma", pr->GetValue("BF", "gamma"));
    m_pars->AddRealVar("BF_pi", pr->GetValue("BF", "pi"));

    // Make coefficient of gamma vs. pi
    for (str hel : {"010", "101"}) {
        m_pars->AddFormulaVar("G_" + hel, "@0 * @1 * @2",
                ParameterList("BF_gamma", "gamma_" + hel + "_acceptance",
                "gamma_" + hel + "_selection"));
        m_pars->AddFormulaVar("P_" + hel, "@0 * @1 * @2",
                ParameterList("BF_pi", "pi_" + hel + "_acceptance",
                "pi_" + hel + "_selection"));
        m_pars->AddFormulaVar("coeff_gamma_" + hel, "@0/(@0 + @1)",
                ParameterList("G_" + hel, "P_" + hel));
        m_pars->AddFormulaVar("coeff_pi_" + hel, "1 - @0",
                ParameterList("coeff_gamma_" + hel));
    }

    // Make each shape
    for (str bod : {"", "4body_"}) {
        for (str parent : {"", "Bs_"}) {
            m_shapes->AddHill(bod + parent + "gamma_010", parent + "gamma_010_a",
                    parent + "gamma_010_b", parent + "gamma_010_csi", "shift",
                    bod + parent + "gamma_010_sigma", parent + "gamma_010_ratio",
                    parent + "gamma_010_frac");
            m_shapes->AddLittleHorns(bod + parent + "gamma_101", parent + "gamma_101_a",
                    parent + "gamma_101_b", parent + "gamma_101_csi", "shift",
                    bod + parent + "gamma_101_sigma", parent + "gamma_101_ratio",
                    parent + "gamma_101_frac", "shiftg");
            m_shapes->AddHorns(bod + parent + "pi_010", parent + "pi_010_a",
                    parent + "pi_010_b", parent + "pi_010_csi", "shift",
                    bod + parent + "pi_010_sigma", parent + "pi_010_ratio",
                    parent + "pi_010_frac");
            m_shapes->AddHill(bod + parent + "pi_101", parent + "pi_101_a",
                    parent + "pi_101_b", parent + "pi_101_csi", "shift",
                    bod + parent + "pi_101_sigma", parent + "pi_101_ratio",
                    parent + "pi_101_frac");

            // Combine gamma and pi shapes
            for (str hel : {"010", "101"}) {
                m_shapes->CombineShapes(bod + parent + "low_" + hel, {
                        {bod + parent + "gamma_" + hel, "coeff_gamma_" + hel},
                        {bod + parent + "pi_" + hel, "coeff_pi_" + hel}
                        });
            }
        }
    }

    // Make floating helicity fractions
    for (str mode : {"Kpi", "piK", "GLW", "Kpipipi", "piKpipi", "pipipipi"}) {

        // Overall fraction
        m_pars->AddRealVar("low_frac_010_" + mode, 0.5, 0, 1);

        // Different fractions for plus and minus
        if (mode != "Kpi" && mode != "Kpipipi") {
            m_pars->AddRealVar("low_frac_010_" + mode + "_plus", 0.5, 0, 1);
            m_pars->AddRealVar("low_frac_010_" + mode + "_minus", 0.5, 0, 1);
        }
    }

    // Same helicity fraction in plus and minus for favoured modes
    for (str fav : {"Kpi", "Kpipipi"}) {
        for (str sign : {"plus", "minus"}) {
            m_pars->AddShared("low_frac_010_" + fav + "_" + sign,
                    "low_frac_010_" + fav);
        }
    }

    // KK and pipi share GLW fraction
    for (str GLW : {"KK", "pipi"}) {
        for (str sign : {"", "_plus", "_minus"}) {
            m_pars->AddShared("low_frac_010_" + GLW + sign, 
                    "low_frac_010_GLW" + sign);
        }
    }

    // Make overall B0 -> D*K*0 shapes
    for (str sign : {"", "_plus", "_minus"}) {
        for (str mode : {"Kpi", "piK", "KK", "pipi"}) {
            m_shapes->CombineShapes("low_" + mode + sign, "low_010", "low_101",
                    "low_frac_010_" + mode);
        } 
        for (str mode : {"Kpipipi", "piKpipi", "pipipipi"}) {
            m_shapes->CombineShapes("low_" + mode + sign, "4body_low_010", 
                    "4body_low_101", "low_frac_010_" + mode);
        }
    }

    // Make overall Bs->D*K*0 shapes
    m_pars->AddRealVar("Bs_low_frac_010", 0.5, 0, 1);
    for (str bod : {"", "4body_"}) {
        m_shapes->CombineShapes(bod + "Bs_low", bod + "Bs_low_010",
                bod + "Bs_low_101", "Bs_low_frac_010");
    }

    // Make yields
    // Ratio between low mass and signal
    m_pars->AddRealVar("BF_R_low_vs_signal", 1.4, 1, 2);
    for (str fav : {"Kpi", "Kpipipi"}) {

        // Make asymmetry
        m_pars->AddRealVar("A_low_" + fav, 0, -1, 1);

        // Get raw yields
        m_pars->AddProductVar("N_low_" + fav, "BF_R_low_vs_signal", "N_signal_" + fav);
        m_pars->AddFormulaVar("N_low_" + fav + "_plus", "@0 * (1 - @1)/2",
                ParameterList("N_low_" + fav, "A_low_" + fav));
        m_pars->AddFormulaVar("N_low_" + fav + "_minus", "@0 * (1 - @1)/(2 * @2)",
                ParameterList("N_low_" + fav, "A_low_" + fav, 
                    "a_corr_" + fav));
    }

    // Make KK/pipi/pipipipi yields
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Make blind ratio and asymmetry
        m_pars->AddRealVar("R_low_" + mode, 1, 0.5, 1.5);
        m_pars->AddRealVar("A_low_" + mode, 0, -1, 1);

        // Calculate raw yields
        std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
        m_pars->AddFormulaVar("N_low_" + mode, "@0 * @1 / @2",
                ParameterList("N_low_" + fav, "R_low_" + mode,
                    "R_corr_" + mode));
        m_pars->AddFormulaVar("N_low_" + mode + "_plus", 
                "@0 * @1 * (1 - @2) / (2 * @3)", ParameterList("N_low_" + fav,
                    "R_low_" + mode, "A_low_" + mode, "R_corr_" + mode));
        m_pars->AddFormulaVar("N_low_" + mode + "_minus", 
                "@0 * @1 * (1 + @2) / (2 * @3 * @4)", ParameterList("N_low_" + fav,
                    "R_low_" + mode, "A_low_" + mode, "R_corr_" + mode,
                    "a_corr_" + mode));
    }

    // Make piK/piKpipi yields
    for (str mode : {"piK", "piKpipi"}) {

        // Yield ratios
        std::string fav = (mode == "piK") ? "Kpi" : "Kpipipi";
        for (str sign : {"", "_plus", "_minus"}) {
            m_pars->AddRealVar("R_low_" + mode + sign, 0.06, 0, 1);
        }

        // Get yields from ratios
        m_pars->AddProductVar("N_low_" + mode, "R_low_" + mode, 
                "N_low_" + fav);
        m_pars->AddFormulaVar("N_low_" + mode + "_plus", "@0 * @1 / @2",
                ParameterList("N_low_" + fav + "_plus", "R_low_" + mode + 
                    "_plus", "a_det_" + mode));
        m_pars->AddFormulaVar("N_low_" + mode + "_minus", "@0 * @1 / @2",
                ParameterList("N_low_" + fav + "_minus", "R_low_" + mode + 
                    "_minus", "a_det_" + mode));
    }

    // Bs low mass yields
    // Ratio between Bs low mass and Bs signal
    m_pars->AddRealVar("BF_R_low_vs_signal_Bs", 1.5, 1, 2);
    for (str sup : {"piK", "piKpipi"}) {

        // Make asymmetry
        m_pars->AddRealVar("A_Bs_low_" + sup, 0, -1, 1);

        // Get raw yields
        m_pars->AddProductVar("N_Bs_low_" + sup, "BF_R_low_vs_signal_Bs", "N_Bs_" + sup);
        m_pars->AddFormulaVar("N_Bs_low_" + sup + "_minus", "@0 * (1 - @1)/2",
                ParameterList("N_Bs_low_" + sup, "A_Bs_low_" + sup));
        m_pars->AddFormulaVar("N_Bs_low_" + sup + "_plus", "@0 * (1 - @1)/(2 * @2)",
                ParameterList("N_Bs_low_" + sup, "A_Bs_low_" + sup, 
                    "a_corr_" + sup + "_s"));
    }

    // KK, pipi, pipipipi Bs yields (completely constrained)
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Make ratio and asymmetry (fixed)
        m_pars->AddRealVar("R_Bs_low_" + mode, 1);
        m_pars->AddRealVar("A_Bs_low_" + mode, 0);

        // Calculate raw yields
        std::string sup = (mode == "pipipipi") ? "piKpipi" : "piK";
        std::string R_mode = (mode == "pipipipi") ? "pipi" : mode;
        m_pars->AddFormulaVar("N_Bs_low_" + mode, "@0 * @1 / @2",
                ParameterList("N_Bs_low_" + sup, "R_Bs_low_" + mode,
                    "R_corr_" + R_mode));
        m_pars->AddFormulaVar("N_Bs_low_" + mode + "_minus", 
                "@0 * @1 * (1 - @2) / (2 * @3)", ParameterList("N_Bs_low_" + sup,
                    "R_Bs_low_" + mode, "A_Bs_low_" + mode, "R_corr_" + R_mode));
        m_pars->AddFormulaVar("N_Bs_low_" + mode + "_plus", 
                "@0 * @1 * (1 + @2) / (2 * @3 * @4)", ParameterList("N_Bs_low_" + sup,
                    "R_Bs_low_" + mode, "A_Bs_low_" + mode, "R_corr_" + R_mode,
                    "a_corr_" + mode + "_s"));
    }
}


// ============================
// Make B -> Drho0 mis-ID shape
// ============================
void DataPdfMaker::MakeRhoShape() {

    // Read shape parameters from file
    ParameterReader * pr = new ParameterReader("../Fit_monte_carlo/Results/");
    pr->ReadParameters("rho", "rho.param");
    for (str par : {"alpha_L", "alpha_R", "n_L", "n_R", "sigma_L", "sigma_ratio", 
            "frac"}) {
        m_pars->AddRealVar("rho_" + par, pr->GetValue("rho", par));
    }

    // Get mean and shift it
    m_pars->AddRealVar("rho_mean_preshift", pr->GetValue("rho", "mean"));
    m_pars->AddSummedVar("rho_mean", "rho_mean_preshift", "shift");

    // Calculate sigma_R
    m_pars->AddProductVar("rho_sigma_R", "rho_sigma_L", "rho_sigma_ratio");

    // Smear width for 4-body
    for (str side : {"_L", "_R"}) {
        m_pars->AddProductVar("4body_rho_sigma" + side, "rho_sigma" + side,
                "four_vs_two_body_ratio");
    }

    // Make shapes
    for (str bod : {"", "4body_"}) {
        for (str side : {"_L", "_R"}) {
            m_shapes->AddCrystalBall(bod + "rho_CB" + side, "rho_mean",
                    bod + "rho_sigma" + side, "rho_alpha" + side, "rho_n" + side);
        }
        m_shapes->CombineShapes(bod + "rho", bod + "rho_CB_L", bod + "rho_CB_R",
                "rho_frac");
    }

    // Make yields
    // Favoured mode yields
    for (str fav : {"Kpipipi", "Kpi"}) {
        double max_fav = GetMaxYield(fav);
        m_pars->AddRealVar("N_rho_" + fav, max_fav/100, 0, max_fav/20);
        for (str sign : {"_plus", "_minus"}) {
            m_pars->AddFormulaVar("N_rho_" + fav + sign, "@0/2", 
                    ParameterList("N_rho_" + fav));
        }
    }

    // Share with suppressed mode
    for (str sup : {"piKpipi", "piK"}) {
        std::string fav = (sup == "piK") ? "Kpi" : "Kpipipi";
        for (str sign : {"", "_plus", "_minus"}){
            m_pars->AddShared("N_rho_" + sup + sign, "N_rho_" + fav + sign);
        }
    }

    // Fix KK/pipi/pipipipi using ratio correction
    for (str mode : {"KK", "pipi", "pipipipi"}) {
        std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
        m_pars->AddRealVar("R_rho_" + mode, 1);
        m_pars->AddFormulaVar("N_rho_" + mode, "@0 * @1 / @2",
                ParameterList("N_rho_" + fav, "R_rho_" + mode,
                    "R_corr_" + mode));
        for (str sign : {"_plus", "_minus"}) {
            m_pars->AddFormulaVar("N_rho_" + mode + sign, "@0/2", 
                    ParameterList("N_rho_" + mode));
        }
    }
}


// ======================================
// Make shape for B+ -> DKpipi background
// ======================================
void DataPdfMaker::MakeDKpipiShape() {

    // Read in parameters
    ParameterReader * pr = new ParameterReader("../Fit_monte_carlo/Results/");
    pr->ReadParameters("DKpipi", "DKpipi.param");

    // Get RooHILL parameters
    for (str par : {"a", "b", "csi", "sigma", "ratio", "frac"}) {
        m_pars->AddRealVar("DKpipi_" + par, pr->GetValue("DKpipi", par));
    }

    // Get Gaussian parameters
    for (str gauss : {"_gauss1", "_gauss2"}) {
        for (str par : {"sigma", "f"}) {
            m_pars->AddRealVar("DKpipi_" + par + gauss, pr->GetValue("DKpipi",
                        par + gauss));
        }

        // Shift Gaussian means
        m_pars->AddRealVar("DKpipi_mean" + gauss + "_preshift", 
                pr->GetValue("DKpipi", "mean" + gauss));
        m_pars->AddSummedVar("DKpipi_mean" + gauss, "DKpipi_mean" + gauss + 
                "_preshift", "shift");
    }

    // Smear sigma for four body version
    for (str sigma : {"sigma", "sigma_gauss1", "sigma_gauss2"}) {
        m_pars->AddProductVar("4body_DKpipi_" + sigma, "DKpipi_" + sigma,
                "four_vs_two_body_ratio");
    }

    // Make shapes
    for (str bod : {"", "4body_"}) {
        for (str gauss : {"_gauss1", "_gauss2"}) {
            m_shapes->AddGaussian(bod + "DKpipi" + gauss, "DKpipi_mean" + gauss,
                    bod + "DKpipi_sigma" + gauss);
        }
        m_shapes->AddHill(bod + "DKpipi_hill", "DKpipi_a", "DKpipi_b",
                "DKpipi_csi", "shift", bod + "DKpipi_sigma", "DKpipi_ratio",
                "DKpipi_frac");
        m_shapes->CombineShapes(bod + "DKpipi", {
                {bod + "DKpipi_gauss1", "DKpipi_f_gauss1"},
                {bod + "DKpipi_gauss2", "DKpipi_f_gauss2"},
                {bod + "DKpipi_hill", ""}});
    }

    // Make yields
    // Ratio between DKpipi and signal
    m_pars->AddRealVar("BF_R_DKpipi_vs_signal", 0.3, 0, 1);
    for (str fav : {"Kpi", "Kpipipi"}) {

        // Make asymmetry
        m_pars->AddRealVar("A_DKpipi_" + fav, 0);

        // Get raw yields
        m_pars->AddProductVar("N_DKpipi_" + fav, "BF_R_DKpipi_vs_signal", "N_signal_" + fav);
        m_pars->AddFormulaVar("N_DKpipi_" + fav + "_plus", "@0 * (1 - @1)/2",
                ParameterList("N_DKpipi_" + fav, "A_DKpipi_" + fav));
        m_pars->AddFormulaVar("N_DKpipi_" + fav + "_minus", "@0 * (1 - @1)/(2 * @2)",
                ParameterList("N_DKpipi_" + fav, "A_DKpipi_" + fav, 
                    "a_corr_" + fav));
    }

    // Make KK/pipi/pipipipi yields
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Make blind ratio and asymmetry
        m_pars->AddRealVar("R_DKpipi_" + mode, 1, 0.5, 1.5);
        m_pars->AddRealVar("A_DKpipi_" + mode, 0, -1, 1);

        // Calculate raw yields
        std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
        m_pars->AddFormulaVar("N_DKpipi_" + mode, "@0 * @1 / @2",
                ParameterList("N_DKpipi_" + fav, "R_DKpipi_" + mode,
                    "R_corr_" + mode));
        m_pars->AddFormulaVar("N_DKpipi_" + mode + "_plus", 
                "@0 * @1 * (1 - @2) / (2 * @3)", ParameterList("N_DKpipi_" + fav,
                    "R_DKpipi_" + mode, "A_DKpipi_" + mode, "R_corr_" + mode));
        m_pars->AddFormulaVar("N_DKpipi_" + mode + "_minus", 
                "@0 * @1 * (1 + @2) / (2 * @3 * @4)", ParameterList("N_DKpipi_" + fav,
                    "R_DKpipi_" + mode, "A_DKpipi_" + mode, "R_corr_" + mode,
                    "a_corr_" + mode));
    }

    // Make piK/piKpipi yields
    for (str mode : {"piK", "piKpipi"}) {

        // Yield ratios
        std::string fav = (mode == "piK") ? "Kpi" : "Kpipipi";
        for (str sign : {"", "_plus", "_minus"}) {
            m_pars->AddRealVar("R_DKpipi_" + mode + sign, 0.06, 0, 1);
        }

        // Get yields from ratios
        m_pars->AddProductVar("N_DKpipi_" + mode, "R_DKpipi_" + mode, 
                "N_DKpipi_" + fav);
        m_pars->AddFormulaVar("N_DKpipi_" + mode + "_plus", "@0 * @1 / @2",
                ParameterList("N_DKpipi_" + fav + "_plus", "R_DKpipi_" + mode + 
                    "_plus", "a_det_" + mode));
        m_pars->AddFormulaVar("N_DKpipi_" + mode + "_minus", "@0 * @1 / @2",
                ParameterList("N_DKpipi_" + fav + "_minus", "R_DKpipi_" + mode + 
                    "_minus", "a_det_" + mode));
    }
}


// =========================
// Make combinatorial shapes
// =========================
void DataPdfMaker::MakeCombiShape() {

    // Make slopes
    for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        m_pars->AddRealVar("slope_" + mode, -0.005, -0.01, 0);
    }

    // Share slopes between ADS modes
    for (str ext : {"", "pipi"}) {
        m_pars->AddShared("slope_piK" + ext, "slope_Kpi" + ext);
    }

    // Make exponential shapes
    for (str mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi", "pipipipi"}) {
        m_shapes->AddExponential("expo_" + mode, "slope_" + mode);
    }

    // Make yields
    for (str mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi", "pipipipi"}) {
        double max = GetMaxYield(mode);
        m_pars->AddRealVar("N_expo_" + mode, max/3, 0, max);
        for (str sign : {"_plus", "_minus"}) {
            m_pars->AddFormulaVar("N_expo_" + mode + sign, "@0/2",
                    ParameterList("N_expo_" + mode));
        }
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
        shapes.emplace(type + "signal", "N_signal_" + mode);
        shapes.emplace(type + "rho", "N_rho_" + mode);
        shapes.emplace("expo_" + mode_short, "N_expo_" + mode);
        shapes.emplace("low_" + mode, "N_low_" + mode);

        // Shapes for suppressed modes only
        if (!is_favoured) {
            shapes.emplace(type + "Bs", "N_Bs_" + mode);
            shapes.emplace(type + "Bs_low", "N_Bs_low_" + mode);
        }

        // DKpipi shape
        shapes.emplace(type + "DKpipi", "N_DKpipi_" + mode);

        // Make the shape
        m_shapes->CombineShapes(mode, shapes);
    }
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


// =========================================
// Calculate and print corrected asymmetries
// =========================================
void DataPdfMaker::PrintAsymmetries() {

    // Not for split
    if (!IsSplit()) {
        return;
    }

    // Detection asymmetry
    m_pars->AddRealVar("A_det", -0.0105);

    // Kpi and Kpipipi
    std::cout << "\nAsymmetries:" << std::endl;
    for (str mode : {"Kpi", "Kpipipi"}) {
        for (str shape : {"signal", "low"}) {
            std::string raw_name = "A_" + mode + "_" + shape;
            std::string phys_name = raw_name + "_phys";
            m_pars->AddFormulaVar(phys_name, "@0 - 2 * @1", 
                    ParameterList(raw_name, "A_det"));
            RooRealVar * asym = (RooRealVar*)m_pars->Get(phys_name);
            RooRealVar * asym_raw = (RooRealVar*)m_pars->Get(raw_name);
            std::cout << "A_" << mode << "_" << shape << ": " << asym->getVal()
                << " +/- " << asym_raw->getError() << std::endl;
        }
    }
    // GLW modes
    for (str mode : {"KK", "pipi", "pipipipi"}) {
        for (str shape : {"low", "Bs_low", "Bs", "DKpipi"}) {
            std::string raw_name = "A_" + mode + "_" + shape;
            std::string phys_name = raw_name + "_phys";
            m_pars->AddFormulaVar(phys_name, "@0 - @1", 
                    ParameterList(raw_name, "A_det"));
            RooRealVar * asym = (RooRealVar*)m_pars->Get(phys_name);
            RooRealVar * asym_raw = (RooRealVar*)m_pars->Get(raw_name);
            std::cout << "A_" << mode << "_" << shape << ": " << asym->getVal()
                << " +/- " << asym_raw->getError() << std::endl;
        }
    }
}


// ====================================
// Calculate and print corrected ratios
// ====================================
void DataPdfMaker::PrintRatios() {

    // Correction factors
    m_pars->AddRealVar("sel_eff_Kpi", 0.01545);
    m_pars->AddRealVar("sel_eff_KK", 0.01608);
    m_pars->AddRealVar("sel_eff_pipi", 0.01773);
    m_pars->AddRealVar("sel_eff_Kpipipi", 0.04575);
    m_pars->AddRealVar("sel_eff_pipipipi", 0.05199);
    m_pars->AddRealVar("PID_eff_Kpi", 0.8699);
    m_pars->AddRealVar("PID_eff_KK", 0.8472);
    m_pars->AddRealVar("PID_eff_pipi", 0.8249);
    m_pars->AddRealVar("BF_Kpi", 0.0389);
    m_pars->AddRealVar("BF_KK", 0.00397);
    m_pars->AddRealVar("BF_pipi", 0.001407);
    m_pars->AddRealVar("BF_Kpipipi", 0.0811);
    m_pars->AddRealVar("BF_pipipipi", 0.00745);

    // Calculate and print
    std::cout << "\nRatios:" << std::endl;
    // KK and pipi
    for (str mode : {"KK", "pipi"}) {
        for (str shape : {"_vs_Kpi_low", "_vs_Kpi_DKpipi", "_vs_piK_Bs"}) {
            std::string raw_name = "R_" + mode + shape;
            std::string phys_name = raw_name + "_phys";
            m_pars->AddFormulaVar(phys_name, "@0 * @1/@2 * @3/@4 * @5/@6",
                    ParameterList(raw_name, "sel_eff_Kpi", "sel_eff_" + mode,
                        "PID_eff_Kpi", "PID_eff_" + mode, "BF_Kpi", "BF_" + mode));
            RooRealVar * ratio = (RooRealVar*)m_pars->Get(phys_name);
            RooRealVar * ratio_raw = (RooRealVar*)m_pars->Get(raw_name);
            std::cout << raw_name << ": " << ratio->getVal() << " +/- " << 
                ratio_raw->getError() * ratio->getVal()/ratio_raw->getVal() << std::endl;
        }
    }

    // pipipipi
    for (str shape : {"_vs_Kpipipi_low", "_vs_Kpipipi_DKpipi", "_vs_piKpipi_Bs"}) {
        std::string raw_name = "R_pipipipi" + shape;
        std::string phys_name = raw_name + "_phys";
        m_pars->AddFormulaVar(phys_name, "@0 * @1/@2 * @3/@4",
                ParameterList(raw_name, "sel_eff_Kpipipi", "sel_eff_pipipipi",
                    "BF_Kpipipi", "BF_pipipipi"));
        RooRealVar * ratio = (RooRealVar*)m_pars->Get(phys_name);
        RooRealVar * ratio_raw = (RooRealVar*)m_pars->Get(raw_name);
        std::cout << raw_name << ": " << ratio->getVal() << " +/- " << 
            ratio_raw->getError() * ratio->getVal()/ratio_raw->getVal() << std::endl;
    }
}


// ========================
// Print yield of each mode
// ========================
void DataPdfMaker::PrintYields(RooFitResult * r) {

    // Kpi/Kpipipi
    std::cout << "\nYields: " << std::endl;
    for (str mode : {"Kpi", "Kpipipi"}) {
        for (str shape : {"signal", "expo", "rho", "low", "DKpipi"}) {
            std::string name = "N_" + shape + "_" + mode;
            if (!IsSplit()) {
                std::cout << name << " = " << m_pars->GetValue(name) << " +/- " <<
                    m_pars->Get(name)->getPropagatedError(*r) << std::endl;
            } else {
                for (str sign : {"_plus", "_minus"}) {
                    std::cout << name << sign << " = " << 
                        m_pars->GetValue(name + sign) << " +/- " <<
                        m_pars->Get(name + sign)->getPropagatedError(*r) << std::endl;
                }
            }
        }
    }

    // Others
    for (str mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
        for (str shape : {"expo", "rho", "low", "Bs", "Bs_low", "DKpipi"}) {
            std::string name = "N_" + shape + "_" + mode;
            if (!IsSplit()) {
                std::cout << name << " = " << m_pars->GetValue(name) << " +/- " <<
                    m_pars->Get(name)->getPropagatedError(*r) << std::endl;
            } else {
                for (str sign : {"_plus", "_minus"}) {
                    std::cout << name << sign << " = " << 
                        m_pars->GetValue(name + sign) << " +/- " <<
                        m_pars->Get(name + sign)->getPropagatedError(*r) << std::endl;
                }
            }
        }
    }
}
