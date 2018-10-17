#include "ParameterReader.hpp"
#include "DataPdfMaker.hpp" 
#include "RooFormulaVar.h"
#include "RooDataHist.h"
#include "RooAbsData.h"

#include <fstream>

typedef std::string str;

// ===========
// Constructor
// ===========
DataPdfMaker::DataPdfMaker(RooRealVar * x, RooCategory * cat, bool blind) :
    ShapeMakerBase("pdf", x, cat),
    m_blind(blind),
    m_asyms_made(false),
    m_ratios_made(false),
    m_sep_R(false) {
    
        m_splitRuns = SplitRuns();
}

DataPdfMaker::DataPdfMaker(std::string name, RooRealVar * x, RooCategory * cat, bool blind) : 
    ShapeMakerBase(name, x, cat),
    m_blind(blind),
    m_sep_R(false) {

        m_splitRuns = SplitRuns();
}


// =====================
// Override shape making
// =====================
void DataPdfMaker::MakeShape() {
    ShapeMakerBase::MakeShape();
    for (auto par : m_zero_pars) {
        m_pars->ChangeValue(par, 0);
        m_pars->SetConstant(par);
    }
}


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

    // Make parameter reader
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/"
            "B02DKstar/");
    
    // Smear factor for 4-body shapes
    pr->ReadParameters("four_vs_two", "Parameters/four_body_width_ratio.param");
    m_pars->AddRealVar("four_vs_two_body_ratio", pr->GetValue("four_vs_two", "ratio"));
    m_pars->AddRealVar("four_vs_two_body_ratio_floating", 1.06, 0.8, 1.3);

    // Loop through runs
    for (auto run : Runs()) {

        // Asymmetry corrections
        pr->ReadParameters("A_prod" + run, "Asymmetries/Production/Results/"
                "production_asymmetry" + run + ".param");
        pr->ReadParameters("A_det" + run, "Asymmetries/Detection/Results/"
                "A_Kpi" + run + ".param");
        m_pars->AddRealVar("A_prod_B0" + run, pr->GetValue("A_prod" + run, "B0"));
        m_pars->AddRealVar("A_prod_Bs" + run, pr->GetValue("A_prod" + run, "Bs"));
        m_pars->AddRealVar("A_det" + run, pr->GetValue("A_det" + run, "A_Kpi"));

        // Ratio corrections
        // Create for each mode
        for (str eff : {"selection", "acceptance", "PID"}) {
            pr->ReadParameters(eff + run, "Efficiencies/Values/" + eff + "_efficiency" 
                    + run + ".param");
            for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
                m_pars->AddRealVar(eff + "_efficiency_" + mode + run, 
                        pr->GetValue(eff + run, mode));
            }
        }

        // Calculate overall efficiency factor for each mode
        for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
            m_pars->AddFormulaVar("efficiency_" + mode + run, "@0 * @1 * @2",
                    ParameterList("selection_efficiency_" + mode + run,
                        "acceptance_efficiency_" + mode + run, 
                        "PID_efficiency_" + mode + run));
        }

        // PID efficiencies split by B0/B0bar
        for (str parent : {"B0", "B0bar"}) {
            pr->ReadParameters("PID_" + parent + run, "Efficiencies/Values/"
                    "PID_efficiency_" + parent + run + ".param");
            for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
                m_pars->AddRealVar("PID_" + parent + "_" + mode + run,
                        pr->GetValue("PID_" + parent + run, mode));
            }
        }
    }

    // Read in branching fractions
    pr->ReadParameters("BF", "Parameters/branching_fractions.param");
    for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        m_pars->AddRealVar("BF_" + mode, pr->GetValue("BF", mode));
    }

    // Get correction factor to apply to B0bar w.r.t. B0
    for (auto run : Runs()) {
        // Production asymmetry factor
        for (str particle : {"B0", "Bs"}) {
            m_pars->AddFormulaVar("a_prod_" + particle + run, "(1 - @0)/(1 + @0)",
                    ParameterList("A_prod_" + particle + run));
        }

        // Set detection asymmetry for each mode
        for (str mode : {"Kpi", "Kpipipi"}) {
            m_pars->AddFormulaVar("A_det_" + mode + run, "2 * @0", 
                    ParameterList("A_det" + run));
        }
        for (str mode : {"KK", "pipi", "pipipipi"}) {
            m_pars->AddFormulaVar("A_det_" + mode + run, "1 * @0", 
                    ParameterList("A_det" + run));
            m_pars->AddFormulaVar("A_det_" + mode + "_s" + run, "-1 * @0", 
                    ParameterList("A_det" + run));
        }
        for (str mode : {"piK", "piKpipi"}) {
            m_pars->AddFormulaVar("a_det_" + mode + run, "(1  - @0)/(1 + @0)",
                    ParameterList("A_det" + run));
            m_pars->AddFormulaVar("A_det_" + mode + "_s" + run, "0 * @0", 
                    ParameterList("A_det" + run));
        }

        // Calculate detection asymmetry factor
        for (str mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi", "pipipipi"}) {
            if (mode != "piK" && mode != "piKpipi") {
                m_pars->AddFormulaVar("a_det_" + mode + run, "(1 - @0)/(1 + @0)",
                        ParameterList("A_det_" + mode + run));
            }
            if (mode != "Kpi" && mode != "Kpipipi") {
                m_pars->AddFormulaVar("a_det_" + mode + "_s" + run, "(1 - @0)/(1 + @0)",
                        ParameterList("A_det_" + mode + "_s" + run));
            }
        }

        // Calculate PID efficiency factors
        for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
            m_pars->AddFormulaVar("a_PID_" + mode + run, "@0/@1", 
                    ParameterList("PID_B0_" + mode + run, "PID_B0bar_" + mode + run));
        }


        // Calculate total correction factor for each mode
        for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
            m_pars->AddFormulaVar("a_corr_" + mode + run, "@0 * @1 * @2",
                    ParameterList("a_PID_" + mode + run, "a_det_" + mode + run,
                        "a_prod_B0" + run));
            // m_pars->AddRealVar("a_corr_" + mode + run, 1);
        }
        for (str mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
            std::string pid_mode = mode;
            if (mode == "piK") pid_mode = "Kpi";
            if (mode == "piKpipi") pid_mode = "Kpipipi";
            // m_pars->AddFormulaVar("a_corr_" + mode + "_s" + run, "@0 * @1 * @2",
                    // ParameterList("a_PID_" + pid_mode + run,
                        // "a_det_" + mode + "_s" + run, "a_prod_Bs" + run));
            m_pars->AddRealVar("a_corr_" + mode + "_s" + run, 1);
        }

        // Calculate ratio correction factor for each mode
        for (str mode : {"KK", "pipi", "pipipipi"}) {
            std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
            m_pars->AddFormulaVar("R_corr_" + mode + run, "(@0 * @1)/(@2 * @3)",
                    ParameterList("efficiency_" + fav + run, "BF_" + fav,
                        "efficiency_" + mode + run, "BF_" + mode));
        }
    }

    // Get d vs. s parameters
    pr->ReadParameters("f", "Parameters/hadronization_fraction.param");
    m_pars->AddRealVar("fs_fd", pr->GetValue("f", "fs_fd"));
    for (auto run : Runs()) {
        pr->ReadParameters("Bs_eff" + run, "Efficiencies/Values/total_efficiency_Bs" + run + ".param");
        m_pars->AddRealVar("tot_eff_Bs" + run, pr->GetValue("Bs_eff" + run, "Kpi"));
        m_pars->AddFormulaVar("R_corr_ds" + run, "@0 * @1 / (@2 * @3)",
        ParameterList("fs_fd", "tot_eff_Bs" + run, "selection_efficiency_Kpi" + run,
            "acceptance_efficiency_Kpi" + run));
    }

    // pr->ReadParameters("lifetime", "Parameters/lifetimes.param");
    // m_pars->AddRealVar("tau_d", pr->GetValue("lifetime", "B0"));
    // m_pars->AddRealVar("tau_s", pr->GetValue("lifetime", "Bs"));

    // Get dilution factor for 4-body
    pr->ReadParameters("dilution", "Parameters/F_CP.param");
    m_pars->AddRealVar("F_CP", pr->GetValue("dilution", "F_CP"));
    m_pars->AddFormulaVar("dilution_factor", "2 * @0 - 1", ParameterList("F_CP"));
    m_pars->AddRealVar("r_B_DKpipi", pr->GetValue("dilution", "r_B_DKpipi"));
    m_pars->AddRealVar("r_B_DKstar", pr->GetValue("dilution", "r_B_DKstar"));
    m_pars->AddFormulaVar("1_plus_rB_2_DKpipi", "1 + @0 * @0", ParameterList("r_B_DKpipi"));
    m_pars->AddFormulaVar("1_plus_rB_2_DKstar", "1 + @0 * @0", ParameterList("r_B_DKstar"));
}


// ==================
// Make signal shapes
// ==================
void DataPdfMaker::MakeSignalShape() {

    // Parameter reader
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Fit_monte_carlo/Results/");

    // Loop through signal shapes
    for (str shape : {"Kpi", "Bs"}) {

        // Get shape name
        std::string name = shape;
        if (shape == "Kpi") name = "signal";

        // Read in constant parameters
        pr->ReadParameters(name, "signal_" + shape + ".param");
        for (str par : {"alpha_L", "alpha_R", "frac", "n_L", "n_R"}) {
            m_pars->AddRealVar(name + "_" + par, pr->GetValue(name, par));
        }

        // Apply global shift to mean
        m_pars->AddRealVar(name + "_mean_preshift", pr->GetValue(name, "mean"));
        m_pars->AddSummedVar(name + "_mean", name + "_mean_preshift", "shift");

        // Float widths
        double sigma_start = pr->GetValue(name, "sigma_L");
        m_pars->AddRealVar(name + "_sigma_L", sigma_start, sigma_start - 10, 23);

        // Make shape
        for (str side : {"_L", "_R"}) {
            m_shapes->AddCrystalBall(name + "_CB" + side, name + "_mean",
                    name + "_sigma_L", name + "_alpha" + side, 
                    name + "_n" + side);
        }
        m_shapes->CombineShapes(name, name + "_CB_L", name + "_CB_R", name + "_frac");
    }

    // Make 4-body signal shapes (adjusted width)
    for (str name : {"signal", "Bs"}) {
	    m_pars->AddProductVar("4body_" + name + "_sigma_L", name + "_sigma_L", 
		    "four_vs_two_body_ratio_floating");
	    for (str side : {"_L", "_R"}) {
            m_shapes->AddCrystalBall("4body_" + name + "_CB" + side, name + "_mean", 
                "4body_" + name + "_sigma_L", name + "_alpha" + side, name + "_n" + side);
            }
	    m_shapes->CombineShapes("4body_" + name, "4body_" + name + "_CB_L", "4body_" + name + "_CB_R", name + "_frac");
    }

    // Make favoured yields
    for (str fav : {"Kpi", "Kpipipi"}) {

        // Asymmetry: shared for Run 1 and Run 2
        m_pars->AddRealVar("A_signal_" + fav, 0, -1, 1);
        // m_pars->AddRealVar("A_signal_" + fav, 0);
        for (auto run : Runs()) {

            // Floating asymmetry and total yield
            double max_fav = GetMaxYield(fav + run);
            m_pars->AddRealVar("N_signal_" + fav + run, max_fav/3, 0, max_fav);

            // Calculate raw yields from these
            m_pars->AddFormulaVar("N_signal_" + fav + run + "_plus", "@0 * (1 - @1)/2",
                    ParameterList("N_signal_" + fav + run, "A_signal_" + fav));
            m_pars->AddFormulaVar("N_signal_" + fav + run + "_minus", 
                    "@0 * (1 + @1)/(2 * @2)", ParameterList("N_signal_" + fav + run, 
                        "A_signal_" + fav, "a_corr_" + fav + run));
        }
    }

    // Make KK/pipi/pipipipi yields
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Separate observables for Run 1 and Run 2
        for (auto run : Runs()) {
            // Make blind ratio and asymmetry
            std::string type = m_blind ? "_blind" : "";
            m_pars->AddRealVar("R_signal_" + mode + run + type, 1, 0, 2);
            m_pars->AddRealVar("A_signal_" + mode + run + type, 0, -1, 1);
            if (m_blind) {
                m_pars->AddUnblindVar("A_signal_" + mode + run, 
                        "A_signal_" + mode + run + "_blind",
                        "blind_" + mode + run + "_asym", 0.005);
                m_pars->AddUnblindVar("R_signal_" + mode + run, 
                        "R_signal_" + mode + run + "_blind",
                        "blind_" + mode + run + "_ratio", 0.005);
            }

            // Calculate raw yields
            std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
            m_pars->AddFormulaVar("N_signal_" + mode + run, "@0 * @1 / @2",
                    ParameterList("N_signal_" + fav + run, "R_signal_" + mode + run,
                        "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_signal_" + mode + run + "_plus", 
                    "@0 * @1 * (1 - @2) / (2 * @3)", 
                    ParameterList("N_signal_" + fav + run, "R_signal_" + mode + run, 
                        "A_signal_" + mode + run, "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_signal_" + mode + run + "_minus", 
                    "@0 * @1 * (1 + @2) / (2 * @3 * @4)", 
                    ParameterList("N_signal_" + fav + run, "R_signal_" + mode + run, 
                        "A_signal_" + mode + run, "R_corr_" + mode + run,
                        "a_corr_" + mode + run));
        }
    }

    // Make piK/piKpipi yields
    for (str mode : {"piK", "piKpipi"}) {

        // Yield ratios: shared across runs
        std::string fav = (mode == "piK") ? "Kpi" : "Kpipipi";
        std::string type = m_blind ? "_blind" : "";
        for (str sign : {"", "_plus", "_minus"}) {
            if (m_sep_R) {
                for (auto run : Runs()) {
                    m_pars->AddRealVar("R_signal_" + mode + run + sign + type, 0.06, -0.05, 1);
                    if (m_blind) {
                        m_pars->AddUnblindVar("R_signal_" + mode + run + sign, 
                                "R_signal_" + mode + run + sign + "_blind", 
                                "blind_" + mode + "_ratio" + sign, 0.01);
                    }
                }
            } else {
                m_pars->AddRealVar("R_signal_" + mode + sign + type, 0.06, -0.05, 1);
                if (m_blind) {
                    m_pars->AddUnblindVar("R_signal_" + mode + sign, 
                            "R_signal_" + mode + sign + "_blind", 
                            "blind_" + mode + "_ratio" + sign, 0.01);
                }
            }
        }

        // Get yields from ratios: split by run
        for (auto run : Runs()) {
            if (m_sep_R) {
                m_pars->AddProductVar("N_signal_" + mode + run, "R_signal_" + mode + run, 
                        "N_signal_" + fav + run);
                m_pars->AddFormulaVar("N_signal_" + mode + run + "_plus", "@0 * @1 / @2",
                        ParameterList("N_signal_" + fav + run + "_plus", 
                            "R_signal_" + mode + run + "_plus", "a_det_" + mode + run));
                m_pars->AddFormulaVar("N_signal_" + mode + run + "_minus", 
                        "@0 * @1 * @2", ParameterList("N_signal_" + fav + run + "_minus",  
                           "R_signal_" + mode + run + "_minus", "a_det_" + mode + run));
            } else {
                m_pars->AddProductVar("N_signal_" + mode + run, "R_signal_" + mode, 
                        "N_signal_" + fav + run);
                m_pars->AddFormulaVar("N_signal_" + mode + run + "_plus", "@0 * @1 / @2",
                        ParameterList("N_signal_" + fav + run + "_plus", 
                            "R_signal_" + mode + "_plus", "a_det_" + mode + run));
                m_pars->AddFormulaVar("N_signal_" + mode + run + "_minus", 
                        "@0 * @1 * @2", ParameterList("N_signal_" + fav + run + "_minus",  
                           "R_signal_" + mode + "_minus", "a_det_" + mode + run));
            }
        }
    }

    // Bs yields
    // Suppressed mode yields and asymmetry
    for (str sup : {"piK", "piKpipi"}) {
            
        // Floating asymmetry and total yield
        m_pars->AddRealVar("A_Bs_" + sup, 0, -1, 1);

        for (auto run : Runs()) {
            double max_sup = GetMaxYield(sup + run);
            m_pars->AddRealVar("N_Bs_" + sup + run, max_sup/3, 0, max_sup);

            // Calculate raw yields from these
            m_pars->AddFormulaVar("N_Bs_" + sup + run + "_minus", "@0 * (1 - @1)/2",
                    ParameterList("N_Bs_" + sup + run, "A_Bs_" + sup));
            m_pars->AddFormulaVar("N_Bs_" + sup + run + "_plus", 
                    "@0 * (1 + @1)/(2 * @2)", ParameterList("N_Bs_" + sup + run, 
                        "A_Bs_" + sup, "a_corr_" + sup + "_s" + run));
        }
    }

    // KK, pipi, pipipipi ds ratios
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Make ratio and asymmetry
        for (auto run : Runs()) {
            m_pars->AddRealVar("A_Bs_" + mode + run, 0, -1, 1);
            // std::string type = m_blind ? "_blind" : "";
            // m_pars->AddRealVar("R_ds_" + mode + run + type, 0.1, 0, 1);
            // if (m_blind) {
                // m_pars->AddUnblindVar("R_ds_" + mode + run,
                        // "R_ds_" + mode + run + "_blind",
                        // "blind_ds_ratio_" + mode + run, 0.01);
            // }

            // // Calculate raw Bs yields from these
            // m_pars->AddFormulaVar("N_Bs_" + mode + run, "@0 * @1 / @2",
                    // ParameterList("N_signal_" + mode + run, "R_corr_ds" + run,
                        // "R_ds_" + mode + run));

            double max_yield = GetMaxYield(mode + run);
            m_pars->AddRealVar("N_Bs_" + mode + run, max_yield/3, 0, max_yield);
            m_pars->AddFormulaVar("N_Bs_" + mode + run + "_plus",
                    "@0 * (1 + @1)/(2 * @2)", ParameterList("N_Bs_" + mode + run,
                        "A_Bs_" + mode + run, "a_corr_" + mode + "_s" + run));
            m_pars->AddFormulaVar("N_Bs_" + mode + run + "_minus", "@0 * (1 - @1)/2",
                    ParameterList("N_Bs_" + mode + run, "A_Bs_" + mode + run));

            // Calculate R_ds from this
            std::string type = m_blind ? "_blind" : "";
            m_pars->AddFormulaVar("R_ds_" + mode + run + type, "@0 * @1 / @2", 
                    ParameterList("R_corr_ds" + run, "N_signal_"+ mode + run,
                        "N_Bs_" + mode + run));
            if (m_blind) {
                m_pars->AddUnblindVar("R_ds_" + mode + run, "R_ds_" + mode + run + type,
                        "blind_ds_ratio_" + mode + run, 0.01);
            }
        }
    }
}


// ==============================
// Make Bs and B0 low mass shapes
// ==============================
void DataPdfMaker::MakeLowMassShape() {

    // Get shape parameters
    ParameterReader * pr = new ParameterReader("../Fit_monte_carlo/Results/");
    // m_pars->AddRealVar("4body_csi_factor", 1, 0.2, 2);
    m_pars->AddRealVar("4body_csi_factor", 1);
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

                // Make adjusted 4-body width and csi
                m_pars->AddProductVar("4body_" + name + "_sigma", name + "_sigma",
                        "four_vs_two_body_ratio");
                m_pars->AddProductVar("4body_" + name + "_csi", name + "_csi",
                        "4body_csi_factor");
            }
        }
    }

    // Make shiftg (fixed to zero) for LittleHorns
    m_pars->AddRealVar("shiftg", 0);

    // Get selection efficiencies 
    pr->ReadParameters("acceptance", "../../Efficiencies/Values/acceptance_lowMass.param");
    for (str mode : {"gamma_010", "gamma_101", "pi_010", "pi_101"}) {
        m_pars->AddRealVar(mode + "_acceptance", pr->GetValue("acceptance", mode));
    }
    for (auto run : Runs()) {
        pr->ReadParameters("selection" + run, 
                "../../Efficiencies/Values/selection_lowMass" + run + ".param");
        for (str mode : {"gamma_010", "gamma_101", "pi_010", "pi_101"}) {
            m_pars->AddRealVar(mode + "_selection" + run, 
                    pr->GetValue("selection" + run, mode));
        }
    }

    // Get branching fractions
    pr->ReadParameters("BF", "../../Parameters/branching_fractions_lowMass.param");
    m_pars->AddRealVar("BF_gamma", pr->GetValue("BF", "gamma"));
    m_pars->AddRealVar("BF_pi", pr->GetValue("BF", "pi"));

    // Make coefficient of gamma vs. pi
    for (str hel : {"010", "101"}) {
        for (auto run : Runs()) {
            m_pars->AddFormulaVar("G_" + hel + run, "@0 * @1 * @2",
                    ParameterList("BF_gamma", "gamma_" + hel + "_acceptance",
                        "gamma_" + hel + "_selection" + run));
            m_pars->AddFormulaVar("P_" + hel + run, "@0 * @1 * @2",
                    ParameterList("BF_pi", "pi_" + hel + "_acceptance",
                        "pi_" + hel + "_selection" + run));
            m_pars->AddFormulaVar("coeff_gamma_" + hel + run, "@0/(@0 + @1)",
                    ParameterList("G_" + hel + run, "P_" + hel + run));
            m_pars->AddFormulaVar("coeff_pi_" + hel + run, "1 - @0",
                    ParameterList("coeff_gamma_" + hel + run));
        }
    }

    // Make each shape
    for (str bod : {"", "4body_"}) {
        for (str parent : {"", "Bs_"}) {
            m_shapes->AddHill(bod + parent + "gamma_010", parent + "gamma_010_a",
                    parent + "gamma_010_b", bod + parent + "gamma_010_csi", "shift",
                    bod + parent + "gamma_010_sigma", parent + "gamma_010_ratio",
                    parent + "gamma_010_frac");
            m_shapes->AddLittleHorns(bod + parent + "gamma_101", parent + "gamma_101_a",
                    parent + "gamma_101_b", bod + parent + "gamma_101_csi", "shift",
                    bod + parent + "gamma_101_sigma", parent + "gamma_101_ratio",
                    parent + "gamma_101_frac", "shiftg");
            m_shapes->AddHorns(bod + parent + "pi_010", parent + "pi_010_a",
                    parent + "pi_010_b", bod + parent + "pi_010_csi", "shift",
                    bod + parent + "pi_010_sigma", parent + "pi_010_ratio",
                    parent + "pi_010_frac");
            m_shapes->AddHill(bod + parent + "pi_101", parent + "pi_101_a",
                    parent + "pi_101_b", bod + parent + "pi_101_csi", "shift",
                    bod + parent + "pi_101_sigma", parent + "pi_101_ratio",
                    parent + "pi_101_frac");

            // Combine gamma and pi shapes
            for (str hel : {"010", "101"}) {
                for (auto run : Runs()) {
                    m_shapes->CombineShapes(bod + parent + "low_" + hel + run, {
                            {bod + parent + "gamma_" + hel, "coeff_gamma_" + hel + run},
                            {bod + parent + "pi_" + hel, "coeff_pi_" + hel + run}
                            });
                }
            }
        }
    }

    // Make floating helicity fractions
    for (str mode : {"Kpi", "GLW"}) {

        // Overall fraction
        m_pars->AddRealVar("low_frac_010_" + mode, 0.7, 0, 0.85);

        // Different fractions for plus and minus
        if (mode != "Kpi" && mode != "Kpipipi") {
            m_pars->AddRealVar("low_frac_010_" + mode + "_plus", 0.7, 0, 1);
            m_pars->AddRealVar("low_frac_010_" + mode + "_minus", 0.7, 0, 1);
        }
    }

    // Kpipipi shares with Kpi
    m_pars->AddShared("low_frac_010_Kpipipi", "low_frac_010_Kpi");
    m_pars->AddShared("low_frac_010_piK", "low_frac_010_Kpi");
    m_pars->AddShared("low_frac_010_piKpipi", "low_frac_010_Kpi");

    // Same helicity fraction in plus and minus for favoured modes
    for (str fav : {"Kpi", "Kpipipi", "piK", "piKpipi"}) {
        for (str sign : {"plus", "minus"}) {
            m_pars->AddShared("low_frac_010_" + fav + "_" + sign,
                    "low_frac_010_" + fav);
        }
    }

    // KK and pipi share GLW fraction
    for (str GLW : {"KK", "pipi", "pipipipi"}) {
        for (str sign : {"", "_plus", "_minus"}) {
            m_pars->AddShared("low_frac_010_" + GLW + sign, 
                    "low_frac_010_GLW" + sign);
        }
    }

    // Bs fraction shared
    m_pars->AddRealVar("Bs_low_frac_010", 0.65, 0, 1);

    // Make combined shapes for each run
    for (auto run : Runs()) {
        // Make overall B0 -> D*K*0 shapes
        for (str sign : {"", "_plus", "_minus"}) {
            for (str mode : {"Kpi", "piK", "KK", "pipi"}) {
                m_shapes->CombineShapes("low_" + mode + run + sign, "low_010" + run, 
                        "low_101" + run, "low_frac_010_" + mode + sign);
            } 
            for (str mode : {"Kpipipi", "piKpipi", "pipipipi"}) {
                m_shapes->CombineShapes("low_" + mode + run + sign, 
                        "4body_low_010" + run, "4body_low_101" + run, 
                        "low_frac_010_" + mode + sign);
            }
        }

        // Make overall Bs->D*K*0 shapes
        for (str bod : {"", "4body_"}) {
            m_shapes->CombineShapes(bod + "Bs_low" + run, bod + "Bs_low_010" + run,
                    bod + "Bs_low_101" + run, "Bs_low_frac_010");
        }
    }

    // Make yields
    // Ratio between low mass and signal: shared for Run 1 and Run 2
    m_pars->AddRealVar("BF_R_low_vs_signal", 1.4, 1, 2);
    for (str fav : {"Kpi", "Kpipipi"}) {

        // Make BF
        // m_pars->AddRealVar("BF_R_low_vs_signal_" + fav, 1.4, 0.7, 2);

        // Make asymmetry (share across runs)
        m_pars->AddRealVar("A_low_" + fav, 0, -1, 1);
        // m_pars->AddRealVar("A_low_" + fav, 0);

        // Get raw yields
        for (auto run : Runs()) {
            m_pars->AddProductVar("N_low_" + fav + run, "BF_R_low_vs_signal", 
                    "N_signal_" + fav + run);
            m_pars->AddFormulaVar("N_low_" + fav + run + "_plus", "@0 * (1 - @1)/2",
                    ParameterList("N_low_" + fav + run, "A_low_" + fav));
            m_pars->AddFormulaVar("N_low_" + fav + run + "_minus", 
                    "@0 * (1 + @1)/(2 * @2)", ParameterList("N_low_" + fav + run, 
                        "A_low_" + fav, "a_corr_" + fav + run));
        }
    }

    // Make KK/pipi/pipipipi yields
    m_pars->AddRealVar("A_low_GLW", 0, -1, 1);
    m_pars->AddProductVar("A_low_pipipipi", "A_low_GLW", "dilution_factor");
    m_pars->AddRealVar("R_low_GLW", 1, 0.3, 2);
    m_pars->AddFormulaVar("R_low_pipipipi", "@0 + (@1 - @0)/@2", 
            ParameterList("1_plus_rB_2_DKstar", "R_low_GLW", "dilution_factor"));
    // m_pars->AddRealVar("R_low_pipipipi", 1, 0.3, 2);
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Calculate raw yields
        std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
        std::string asym_mode = (mode == "pipipipi") ? "pipipipi" : "GLW";
        for (auto run : Runs()) {
            m_pars->AddFormulaVar("N_low_" + mode + run, "@0 * @1 / @2",
                    ParameterList("N_low_" + fav + run, "R_low_" + asym_mode,
                        "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_low_" + mode + run + "_plus", 
                    "@0 * @1 * (1 - @2) / (2 * @3)", ParameterList("N_low_" + fav + run,
                        "R_low_" + asym_mode, "A_low_" + asym_mode, 
                        "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_low_" + mode + run + "_minus", 
                    "@0 * @1 * (1 + @2) / (2 * @3 * @4)", 
                    ParameterList("N_low_" + fav + run, "R_low_" + asym_mode, 
                        "A_low_" + asym_mode, "R_corr_" + mode + run,
                        "a_corr_" + mode + run));
        }
    }

    // Read in predicted values
    pr->ReadParameters("obs", "../../Parameters/predicted_observables.param");

    // Make piK/piKpipi yields
    for (str mode : {"piK", "piKpipi"}) {

        // Yield ratios: read predicted values from file
        std::string fav = (mode == "piK") ? "Kpi" : "Kpipipi";
        std::string extra = (mode == "piK") ? "" : "_K3pi";
        // m_pars->AddRealVar("R_low_" + mode, pr->GetValue("obs", "R_ADS" + extra));
        m_pars->AddRealVar("R_low_" + mode, 0.1, -0.2, 0.3);
        for (str sign : {"_plus", "_minus"}) {
            // m_pars->AddRealVar("R_low_" + mode + sign, pr->GetValue("obs", "R" + sign + extra));
            m_pars->AddRealVar("R_low_" + mode + sign, 0.1, -0.2, 0.3);
        }

        // Get yields from ratios
        for (auto run : Runs()) {
            m_pars->AddProductVar("N_low_" + mode + run, "R_low_" + mode, 
                    "N_low_" + fav + run);
            m_pars->AddFormulaVar("N_low_" + mode + run + "_plus", "@0 * @1 / @2",
                    ParameterList("N_low_" + fav + run + "_plus", "R_low_" + mode + 
                        "_plus", "a_det_" + mode + run));
            m_pars->AddFormulaVar("N_low_" + mode + run + "_minus", "@0 * @1 * @2",
                    ParameterList("N_low_" + fav + run + "_minus", "R_low_" + mode + 
                        "_minus", "a_det_" + mode + run));
        }
    }

    // Bs low mass yields
    // Ratio between Bs low mass and Bs signal
    m_pars->AddRealVar("BF_R_low_vs_signal_Bs", 1.5, 1, 2);
    for (str sup : {"piK", "piKpipi"}) {

        // Make asymmetry
        m_pars->AddRealVar("A_Bs_low_" + sup, 0, -1, 1);

        // Get raw yields
        for (auto run : Runs()) {
            m_pars->AddProductVar("N_Bs_low_" + sup + run, "BF_R_low_vs_signal_Bs", 
                    "N_Bs_" + sup + run);
            m_pars->AddFormulaVar("N_Bs_low_" + sup + run + "_minus", "@0 * (1 - @1)/2",
                    ParameterList("N_Bs_low_" + sup + run, "A_Bs_low_" + sup));
            m_pars->AddFormulaVar("N_Bs_low_" + sup + run + "_plus", "@0 * (1 + @1)/(2 * @2)",
                    ParameterList("N_Bs_low_" + sup + run, "A_Bs_low_" + sup, 
                        "a_corr_" + sup + "_s" + run));
        }
    }

    // KK, pipi, pipipipi Bs low yields (completely constrained)
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Make ratio and asymmetry (fixed)
        // m_pars->AddRealVar("R_Bs_low_" + mode, 1, 0.5, 1.5);
        m_pars->AddRealVar("R_Bs_low_" + mode, 1);
        m_pars->AddRealVar("A_Bs_low_" + mode, 0);

        // Calculate raw yields
        std::string sup = (mode == "pipipipi") ? "piKpipi" : "piK";
        for (auto run : Runs()) {
            m_pars->AddFormulaVar("N_Bs_low_" + mode + run, "@0 * @1 / @2",
                    ParameterList("N_Bs_low_" + sup + run, "R_Bs_low_" + mode,
                        "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_Bs_low_" + mode + run + "_minus", 
                    "@0 * @1 * (1 - @2) / (2 * @3)", 
                    ParameterList("N_Bs_low_" + sup + run, "R_Bs_low_" + mode, 
                        "A_Bs_low_" + mode, "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_Bs_low_" + mode + run + "_plus", 
                    "@0 * @1 * (1 + @2) / (2 * @3 * @4)", 
                    ParameterList("N_Bs_low_" + sup + run, "R_Bs_low_" + mode, 
                        "A_Bs_low_" + mode, "R_corr_" + mode + run,
                        "a_corr_" + mode + "_s" + run));
        }
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
    // pr->ReadParameters("rho_ratios", "../../Parameters/rho_ratios.param");
    // for (str run : {"1", "2"}) {
        // m_pars->AddRealVar("BF_R_rho_Kpi_run" + run, pr->GetValue("rho_ratios", "Kpi_run" + run));
        // m_pars->AddRealVar("BF_R_rho_Kpipipi_run" + run, pr->GetValue("rho_ratios", "Kpipipi_run" + run));
    // }

    // Get PID efficiencies for each run 
    pr->ReadParameters("rho_PID_eff_run1", 
            "../../Efficiencies/Values/PID_efficiency_rho_run1.param");
    pr->ReadParameters("rho_PID_eff_run2", 
            "../../Efficiencies/Values/PID_efficiency_rho_run2.param");

    // Get PID efficiencies for each run
    for (str run : Runs()) {
        m_pars->AddRealVar("rho_PID_eff_Kpi" + run, 
                pr->GetValue("rho_PID_eff" + run, "Kpi"));
    }

    // Make normalisation factor for Run 2 rho yield
    m_pars->AddFormulaVar("rho_run_ratio_Kpi", "@0 * @1 / (@2 * @3)",
            ParameterList("rho_PID_eff_Kpi_run2", "PID_efficiency_Kpi_run1", 
                "rho_PID_eff_Kpi_run1", "PID_efficiency_Kpi_run2"));

    // Make Run 1 rho ratio
    m_pars->AddRealVar("BF_R_rho_run1", 0.07, 0, 0.2);
   
    // Extrapolate Run 2 ratio
    m_pars->AddProductVar("BF_R_rho_run2", "BF_R_rho_run1", "rho_run_ratio_Kpi");

    // // Read rho ratios from file
    // pr->ReadParameters("BF_rho", "../../Parameters/rho_ratios.param");
    // for (auto run : Runs()) {
        // for (str fav : {"Kpi", "Kpipipi"}) {
            // m_pars->AddRealVar("BF_R_rho_" + fav + run,
                    // pr->GetValue("BF_rho", fav + run));
        // }
    // }

    // Calculate yields
    for (auto run : Runs()) {
        for (str fav : {"Kpipipi", "Kpi"}) {
            // m_pars->AddRealVar("BF_R_rho_" + fav + run, 0.07, 0, 0.2);
            m_pars->AddProductVar("N_rho_" + fav + run, "BF_R_rho" + run,
                    "N_signal_" + fav + run);
            // m_pars->AddProductVar("N_rho_" + fav + run, "BF_R_rho_" + fav + run,
                    // "N_signal_" + fav + run);
            for (str sign : {"_plus", "_minus"}) {
                m_pars->AddFormulaVar("N_rho_" + fav + run + sign, "@0/2", 
                        ParameterList("N_rho_" + fav + run));
            }
        }

        // Share with suppressed mode
        for (str sup : {"piKpipi", "piK"}) {
            std::string fav = (sup == "piK") ? "Kpi" : "Kpipipi";
            for (str sign : {"", "_plus", "_minus"}){
                m_pars->AddShared("N_rho_" + sup + run + sign, 
                        "N_rho_" + fav + run + sign);
            }
        }
    }

    // Fix KK/pipi/pipipipi using ratio correction
    for (str mode : {"KK", "pipi", "pipipipi"}) {
        std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
        std::string sup = (mode == "pipipipi") ? "piKpipi" : "piK";
        m_pars->AddRealVar("R_rho_" + mode, 1);
        for (auto run : Runs()) {
            // m_pars->AddFormulaVar("N_rho_" + mode + run, "@0 * @1 * @2 / @3",
                    // ParameterList("N_rho_" + fav + run, "R_rho_" + mode,
                        // "N_Bs_" + mode + run, "N_Bs_" + sup + run));
            m_pars->AddFormulaVar("N_rho_" + mode + run, "@0 * @1 / @2",
                    ParameterList("N_rho_" + fav + run, "R_rho_" + mode,
                        "R_corr_" + mode + run));
            for (str sign : {"_plus", "_minus"}) {
                m_pars->AddFormulaVar("N_rho_" + mode + run + sign, "@0/2", 
                        ParameterList("N_rho_" + mode + run));
            }
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
    m_pars->AddRealVar("BF_R_DKpipi_vs_signal", 0.1, 0, 0.4);
    for (str fav : {"Kpi", "Kpipipi"}) {

        // Make ratio
        // m_pars->AddRealVar("BF_R_DKpipi_vs_signal_" + fav, 0.1, 0, 0.8);

        // Make asymmetry
        m_pars->AddRealVar("A_DKpipi_" + fav, 0);

        // Get raw yields
        for (auto run : Runs()) {
            m_pars->AddProductVar("N_DKpipi_" + fav + run, 
                    "BF_R_DKpipi_vs_signal", "N_signal_" + fav + run);
            m_pars->AddFormulaVar("N_DKpipi_" + fav + run + "_plus", "@0 * (1 - @1)/2",
                    ParameterList("N_DKpipi_" + fav + run, "A_DKpipi_" + fav));
            m_pars->AddFormulaVar("N_DKpipi_" + fav + run + "_minus", 
                    "@0 * (1 + @1)/(2 * @2)", ParameterList("N_DKpipi_" + fav + run, 
                        "A_DKpipi_" + fav, "a_corr_" + fav + run));
        }
    }

    // Make KK/pipi/pipipipi observables
    pr->ReadParameters("DKpipi_obs", "../../Parameters/DKpipi_observables.param");
    m_pars->AddRealVar("A_DKpipi_KK", pr->GetValue("DKpipi_obs", "A_KK"));
    m_pars->AddRealVar("A_DKpipi_pipi", pr->GetValue("DKpipi_obs", "A_pipi"));
    m_pars->AddRealVar("R_DKpipi_GLW", pr->GetValue("DKpipi_obs", "R_CP"));
    m_pars->AddShared("R_DKpipi_KK", "R_DKpipi_GLW");
    m_pars->AddShared("R_DKpipi_pipi", "R_DKpipi_GLW");
    m_pars->AddRealVar("A_DKpipi_pipipipi", pr->GetValue("DKpipi_obs", "A_4pi"));
    m_pars->AddRealVar("R_DKpipi_pipipipi", pr->GetValue("DKpipi_obs", "R_4pi"));

    // Calculate derived yields
    for (str mode : {"KK", "pipi", "pipipipi"}) {

        // Make ratio and asymmetry
        // std::string asym_mode = (mode == "pipipipi") ? "pipipipi" : "GLW";
        std::string asym_mode = mode;

        // Calculate raw yields
        std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
        for (auto run : Runs()) {
            m_pars->AddFormulaVar("N_DKpipi_" + mode + run, "@0 * @1 / @2",
                    ParameterList("N_DKpipi_" + fav + run, "R_DKpipi_" + asym_mode,
                        "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_DKpipi_" + mode + run + "_plus", 
                    "@0 * @1 * (1 - @2) / (2 * @3)", ParameterList("N_DKpipi_" + fav + run,
                        "R_DKpipi_" + asym_mode, "A_DKpipi_" + asym_mode, 
                        "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_DKpipi_" + mode + run + "_minus", 
                    "@0 * @1 * (1 + @2) / (2 * @3 * @4)", 
                    ParameterList("N_DKpipi_" + fav + run, "R_DKpipi_" + asym_mode, 
                        "A_DKpipi_" + asym_mode, "R_corr_" + mode + run,
                        "a_corr_" + mode + run));
        }
    }

    // Make piK/piKpipi yields
    for (str mode : {"piK", "piKpipi"}) {

        // Yield ratios
        std::string fav = (mode == "piK") ? "Kpi" : "Kpipipi";
        for (str sign : {"", "_plus", "_minus"}) {
            m_pars->AddRealVar("R_DKpipi_" + mode + sign, 
                    pr->GetValue("DKpipi_obs", "R" + sign + "_" + mode));
        }

        // Get yields from ratios
        for (auto run : Runs()) {
            m_pars->AddProductVar("N_DKpipi_" + mode + run, "R_DKpipi_" + mode, 
                    "N_DKpipi_" + fav + run);
            m_pars->AddFormulaVar("N_DKpipi_" + mode + run + "_plus", "@0 * @1 / @2",
                    ParameterList("N_DKpipi_" + fav + run + "_plus", "R_DKpipi_" + mode + 
                        "_plus", "a_det_" + mode + run));
            m_pars->AddFormulaVar("N_DKpipi_" + mode + run + "_minus", "@0 * @1 * @2",
                    ParameterList("N_DKpipi_" + fav + run + "_minus", 
                        "R_DKpipi_" + mode + "_minus", "a_det_" + mode + run));
        }
    }
}


// =========================
// Make combinatorial shapes
// =========================
void DataPdfMaker::MakeCombiShape() {

    // Make slopes
    for (str mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        m_pars->AddRealVar("slope_" + mode, -0.005, -0.007, 0);
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
        for (auto run : Runs()) {
            double max = GetMaxYield(mode + run);
            m_pars->AddRealVar("N_expo_" + mode + run, max/4, 0, max/2);
            for (str sign : {"_plus", "_minus"}) {
                m_pars->AddFormulaVar("N_expo_" + mode + run + sign, "@0/2",
                        ParameterList("N_expo_" + mode + run));
            }
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

        // Get mode name without plus or minus/run number
        std::string mode_short = mode;
        if (mode.find("_plus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_plus"));
        } else if (mode.find("_minus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_minus"));
        }
        if (mode_short.find("_run") != std::string::npos) {
            mode_short = mode_short.substr(0, mode_short.find("_run"));
        }

        // Get run number
        std::string run_number = "";
        if (mode.find("_run1") != std::string::npos) run_number = "_run1";
        else if (mode.find("_run2") != std::string::npos) run_number = "_run2";

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
            shapes.emplace(type + "Bs_low" + run_number, "N_Bs_low_" + mode);
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


// =================================
// Check whether split by run number
// =================================
bool DataPdfMaker::SplitRuns() {

    // Check mode list for plus and minus labels
    bool split = true;
    for (str mode : m_modes) {
        if (mode.find("_run1") == std::string::npos && 
                mode.find("_run2") == std::string::npos) {
            split = false;
        }
    }
    std::cout << "Split runs: " << split << std::endl;
    return split;
}


// =========================
// Get vector of run numbers
// =========================
std::vector<std::string> DataPdfMaker::Runs() {
    std::vector<std::string> runs;
    if (m_splitRuns) {
        runs.push_back("_run1");
        runs.push_back("_run2");
    } else {
        runs.push_back("");
    }
    return runs;
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
    return GetComponentIntegral(signal_name, "N_signal_" + mode, m_Bmass - m_Brange,
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
    return GetComponentIntegral("expo_" + mode_short, "N_expo_" + mode,
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
void DataPdfMaker::SaveHistograms(std::string filename, RooAbsData * data) {
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

    // Open file for printing
    std::string filename = IsSplit() ? "Results/yields_split.param" : "Results/yields_combined.param";
    std::ofstream file(filename);

    // Kpi/Kpipipi
    for (str mode : {"Kpi", "Kpipipi"}) {
        for (str shape : {"signal", "expo", "rho", "low", "DKpipi"}) {
            for (auto run : Runs()) {
                std::string name = "N_" + shape + "_" + mode + run;
                if (!IsSplit()) {
                    file << name << " " << m_pars->GetValue(name) << " " <<
                        m_pars->Get(name)->getPropagatedError(*r) << std::endl;
                } else {
                    for (str sign : {"_plus", "_minus"}) {
                        file << name << sign << " " << m_pars->GetValue(name + sign) << " " 
                            << m_pars->Get(name + sign)->getPropagatedError(*r) << std::endl;
                    }
                }
            }
        }
    }

    // Others
    for (str mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
        for (str shape : {"expo", "rho", "low", "Bs", "Bs_low", "DKpipi"}) {
            for (auto run : Runs()) {
                std::string name = "N_" + shape + "_" + mode + run;
                if (!IsSplit()) {
                    file << name << " " << m_pars->GetValue(name) << " " <<
                        m_pars->Get(name)->getPropagatedError(*r) << std::endl;
                } else {
                    for (str sign : {"_plus", "_minus"}) {
                        file << name << sign << " " << 
                            m_pars->GetValue(name + sign) << " " <<
                            m_pars->Get(name + sign)->getPropagatedError(*r) << std::endl;
                    }
                }
            }
        }
    }

    file.close();

}


// ===================
// Set a yield to zero
// ===================
void DataPdfMaker::SetZeroYield(std::string mode) {

    MakeShape();
    if (mode == "piK") {
        if (IsSplit()) {
            for (str sign : {"_plus", "_minus"}) {
                m_zero_pars.push_back("R_signal_piK" + sign);
            }
        } else {
            m_zero_pars.push_back("R_signal_piK");
        }

    } else if (mode == "piKpipi") {
        if (IsSplit()) {
            for (str sign : {"_plus", "_minus"}) {
                m_zero_pars.push_back("R_signal_piKpipi" + sign);
            }
        } else {
            m_zero_pars.push_back("R_signal_piKpipi");
        }

    }
    m_shapeMade = false; 

}


// =======================
// Calculate value of R_ds
// =======================
RooFormulaVar * DataPdfMaker::GetR_ds(std::string mode, std::string run) {
    std::string type = m_blind ? "_blind" : "";
    return (RooFormulaVar*)m_pars->Get("R_ds_" + mode + run + type);
}


// ====================================
// Make R± separate for run 1 and run 2
// ====================================
void DataPdfMaker::SeparateRruns() {
    m_sep_R = true;
    m_shapeMade = false;
}
