#include "TRandom.h"

#include "SystematicPdfMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
SystematicPdfMaker::SystematicPdfMaker(SysOption opt, RooRealVar * x, 
        RooCategory * cat, bool blind) : 
    DataPdfMaker("sys", x, cat, blind),
    m_opt(opt) {}


// ==========
// Destructor
// ==========
SystematicPdfMaker::~SystematicPdfMaker() {}


// ========================================
// Override of constant parameters function
// ========================================
void SystematicPdfMaker::MakeShape() {

    // Run default function
    ShapeMakerBase::MakeShape();

    // Adjust parameters
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/");
    switch (m_opt) {

        // fs/fd (hadronization fractions)
        case (SysOption::fs_fd) : 
            {
                pr->ReadParameters("fs_fd", "Parameters/hadronization_fraction.param");
                m_pars->AdjustValue("fs_fd", pr->GetError("fs_fd", "fs_fd"), true);
                break;
            }

            // Helicity fraction for piK
        case (SysOption::low_frac_piK) :
            {
                gRandom->SetSeed(0);
                double random = gRandom->Rndm();
                std::cout << "Changing to " << random << std::endl;
                m_pars->ChangeValue("low_frac_010_piK", random);
                m_pars->SetConstant("low_frac_010_piK");
                break;
            }

            // Branching ratios
        case (SysOption::branching_ratios) :
            {
                pr->ReadParameters("BF", "Parameters/branching_fractions.param");    
                for (std::string mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
                    m_pars->AdjustValue("BF_" + mode, pr->GetError("BF", mode), true);
                }
                break;
            }

        // Selection efficiency
        case (SysOption::selection_efficiency) :
            {
                for (std::string run : Runs()) {

                    // Adjust both selection and geometric acceptance efficiency 
                    for (std::string eff : {"acceptance", "selection"}) {

                        // B0
                        pr->ReadParameters(eff + run, "Efficiencies/Values/" + eff + "_efficiency"
                                + run + ".param");
                        for (std::string mode : {"Kpi", "KK", "pipi", "pipipipi", "Kpipipi"}) {
                            m_pars->AdjustValue(eff + "_efficiency_" + mode + run,
                                    pr->GetError(eff + run, mode));
                        }

                        // Bs
                        pr->ReadParameters("Bs_" + eff + run, "Efficiencies/Values/" + eff + "_efficiency"
                                + run + ".param");
                        m_pars->AdjustValue(eff + "_efficiency_Bs" + run, pr->GetError("Bs_" + eff + run, "Kpi"));
                    }
                }
                break;
            }

        // Production asymmetry
        case (SysOption::production_asymmetry) :
            {
                for (std::string run : Runs()) {
                    pr->ReadParameters("A_prod" + run, "Asymmetries/Production/Results/"
                            "production_asymmetry" + run + ".param");
                    m_pars->AdjustValue("A_prod_B0" + run, pr->GetError("A_prod" + run, "B0"));
                    m_pars->AdjustValue("A_prod_Bs" + run, pr->GetError("A_prod" + run, "Bs"));
                }
                break;
            }

        // Detection asymmetry
        case (SysOption::detection_asymmetry) :
            {
                for (std::string run : Runs()) {
                    pr->ReadParameters("A_det" + run, "Asymmetries/Detection/Results/"
                            "A_Kpi" + run + ".param");
                    m_pars->AdjustValue("A_det" + run, pr->GetError("A_det" + run, "A_Kpi"));
                }
                break;
            }

        // Fixed amount of rho background
        case (SysOption::fixed_rho) :
            {
                pr->ReadParameters("BF_rho", "Parameters/rho_ratios.param");
                for (std::string run : {"_run1", "_run2"}) { 
                    for (std::string fav : {"Kpi", "Kpipipi"}) {
                        m_pars->AdjustValue("BF_R_rho_" + fav + run, 
                                pr->GetError("BF_rho", fav + run));
                    }
                }
            break;
            }

        // Fixing 2-vs-4 body ratio
        case (SysOption::four_vs_two) :
            {
                pr->ReadParameters("four_vs_two", "Parameters/four_body_width_ratio.param");
                m_pars->AdjustValue("four_vs_two_body_ratio", pr->GetError("four_vs_two", 
                            "ratio"));
                break;
            }

        // Fixed signal shape parameters
        case (SysOption::signal_shape_pars) :
            {
                std::map<std::string, std::string> names = {
                    {"signal_alpha_L", "alpha_L"},
                    {"signal_alpha_R", "alpha_R"}};
                m_pars->AdjustValues(names, "../Fit_monte_carlo/Histograms/signal_cruijff_Kpi.root");
                std::map<std::string, std::string> names_Bs = {
                    {"Bs_mean_preshift", "mean"},
                    {"Bs_alpha_L", "alpha_L"},
                    {"Bs_alpha_R", "alpha_R"}};
                m_pars->AdjustValues(names_Bs, "../Fit_monte_carlo/Histograms/signal_cruijff_Bs.root");
                break;
            }

        case (SysOption::Bs_low_shape_pars) :
            {
                for (std::string part : {"gamma", "pi"}) {
                    for (std::string hel : {"101", "010"}) {
                        std::string name = "Bs_" + part + "_" + hel;
                        std::map<std::string, std::string> names = {
                            {name + "_a", "a_" + name},
                            {name + "_b", "b_" + name},
                            {name + "_csi", "csi_" + name},
                            {name + "_frac", "frac_" + name},
                            {name + "_ratio", "ratio_" + name},
                            {name + "_sigma", "sigma_" + name}};
                        m_pars->AdjustValues(names, "../Fit_monte_carlo/Histograms/" + name + ".root");
                    }
                }
                break;
            }

        case (SysOption::background_shape_pars) :
            {
                // B0 low mass shape
                for (std::string part : {"gamma", "pi"}) {
                    for (std::string hel : {"101", "010"}) {
                        std::string name = part + "_" + hel;
                        std::map<std::string, std::string> names = {
                            {name + "_csi", "csi_" + name},
                            {name + "_frac", "frac_" + name},
                            {name + "_ratio", "ratio_" + name},
                            {name + "_sigma", "sigma_" + name}};
                        m_pars->AdjustValues(names, "../Fit_monte_carlo/Histograms/" + name + ".root");
                    }
                }

                // Rho shape
                std::map<std::string, std::string> rho_names = {
                    {"rho_alpha_L", "alpha_L"},
                    {"rho_alpha_R", "alpha_R"},
                    {"rho_frac", "frac"},
                    {"rho_mean_preshift", "mean"},
                    {"rho_sigma_L", "sigma_L"},
                    {"rho_sigma_ratio", "sigma_ratio"},
                    {"rho_n_R", "n_R"}};
                m_pars->AdjustValues(rho_names, "../Fit_monte_carlo/Histograms/rho_Kpi.root");
                
                // DKpipi shape
                std::map<std::string, std::string> DKpipi_names = {
                    {"DKpipi_a", "a"},
                    {"DKpipi_sigma", "sigma"},
                    {"DKpipi_ratio", "ratio"},
                    {"DKpipi_frac", "frac"}};
                for (std::string g : {"_gauss1", "_gauss2"}) {
                    DKpipi_names["DKpipi_mean" + g + "_preshift"] = "mean" + g;
                    DKpipi_names["DKpipi_f" + g] = "f" + g;
                    DKpipi_names["DKpipi_sigma" + g] = "sigma" + g;
                }
                m_pars->AdjustValues(DKpipi_names, "../Fit_monte_carlo/Histograms/DKpipi_Kpi.root");
                
                break;
            }


        // Mass difference between B0 and Bs
        case (SysOption::delta_M) :
            {
                pr->ReadParameters("delta_M", "Parameters/delta_M.param");
                m_pars->AdjustValue("delta_M", pr->GetError("delta_M", "delta_M"));
                break;
            }

        // Gamma and pi selection efficiencies
        case (SysOption::gamma_pi_inputs) : 
            {
                // Selection efficiencies
                pr->ReadParameters("acceptance", "Efficiencies/Values/acceptance_lowMass.param");
                for (std::string mode : {"gamma_010", "gamma_101", "pi_010", "pi_101"}) {
                    m_pars->AdjustValue(mode + "_acceptance", pr->GetError("acceptance", mode));
                }
                for (std::string run : Runs()) {
                    pr->ReadParameters("selection" + run, "Efficiencies/Values/selection_lowMass" 
                            + run + ".param");
                    for (std::string mode : {"gamma_010", "gamma_101", "pi_010", "pi_101"}) {
                        m_pars->AdjustValue(mode + "_selection" + run, 
                                pr->GetError("selection" + run, mode));
                    }
                }

                // Branching ratios
                pr->ReadParameters("BF", "Parameters/branching_fractions_lowMass.param");
                m_pars->AdjustValue("BF_gamma", pr->GetError("BF", "gamma"));
                m_pars->AdjustValue("BF_pi", pr->GetError("BF", "pi"));
                break;
            }

        // DKpipi inputs
        case (SysOption::DKpipi_inputs) :
            {
                pr->ReadParameters("DKpipi_obs", "Parameters/DKpipi_observables.param");
                m_pars->AdjustValue("R_DKpipi_piK_plus", pr->GetError("DKpipi_obs", "R_plus_piK") * 2);
                m_pars->AdjustValue("R_DKpipi_piK_minus", pr->GetError("DKpipi_obs", "R_minus_piK") * 2);
                m_pars->AdjustValue("R_DKpipi_piKpipi_plus", pr->GetError("DKpipi_obs", "R_plus_piKpipi") * 2);
                m_pars->AdjustValue("R_DKpipi_piKpipi_minus", pr->GetError("DKpipi_obs", "R_minus_piKpipi") * 2);
                m_pars->AdjustValue("A_DKpipi_KK", pr->GetError("DKpipi_obs", "A_KK") * 2);
                m_pars->AdjustValue("A_DKpipi_pipi", pr->GetError("DKpipi_obs", "A_pipi") * 2);
                m_pars->AdjustValue("R_DKpipi_GLW", pr->GetError("DKpipi_obs", "R_CP") * 2);
                m_pars->AdjustValue("A_DKpipi_pipipipi", pr->GetError("DKpipi_obs", "A_4pi") * 2);
                m_pars->AdjustValue("R_DKpipi_pipipipi", pr->GetError("DKpipi_obs", "R_4pi") * 2);
                break;
            }

        // Rho PID efficiency inputs
        case (SysOption::rho_PID) :
            {
                pr->ReadParameters("rho_PID_eff_run1", 
                        "Efficiencies/Values/PID_efficiency_rho_run1.param");
                pr->ReadParameters("rho_PID_eff_run2", 
                        "Efficiencies/Values/PID_efficiency_rho_run2.param");
                m_pars->AdjustValue("rho_PID_eff_Kpi_run1", 
                        pr->GetError("rho_PID_eff_run1", "Kpi"));
                m_pars->AdjustValue("rho_PID_eff_Kpi_run2", 
                        pr->GetError("rho_PID_eff_run2", "Kpi"));
                break;
            }

        // PID efficiency
        case (SysOption::PID) :
            {
                for (std::string run : Runs()) {
                    for (std::string flav : {"", "_B0", "_B0bar"}) {
                        pr->ReadParameters("PID" + flav + run,
                                "Efficiencies/Values/PID_efficiency" + flav
                                + run + ".param");
                        for (std::string mode : {"Kpi", "KK", "pipi",
                                "Kpipipi", "pipipipi"}){
                            if (mode == "pipipipi" && run != "_run2") continue;
                            if (flav == "") {
                                m_pars->AdjustValue("PID_efficiency_" + mode 
                                        + run, pr->GetError("PID" + run, mode));
                            } else {
                                m_pars->AdjustValue("PID" + flav + "_" + mode + run,
                                        pr->GetError("PID" + flav + run, mode));
                            }
                        }
                    }
                }
                break;
            }

        // Fixing 4pi low observables
        case (SysOption::pipipipi_low) :
            {
                pr->ReadParameters("dilution", "Parameters/F_CP.param");
                m_pars->AdjustValue("F_CP", pr->GetError("dilution", "F_CP"));
                m_pars->AdjustValue("r_B_DKstar", pr->GetError("dilution", "r_B_DKstar"));
                break;
            }


        // None
        case (SysOption::none) :
            { 
                std::cout << "trying a test fit with no adjustment" << std::endl;
            }
    }
}
