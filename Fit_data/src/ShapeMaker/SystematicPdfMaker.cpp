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
                for (std::string run : {"_run1", "_run2"}) {

                    // B0 selection efficiencies
                    for (std::string eff : {"acceptance", "selection"}) {
                        pr->ReadParameters(eff + run, "Efficiencies/Values/" + eff + "_efficiency"
                                + run + ".param");
                        for (std::string mode : {"Kpi", "KK", "pipi", "pipipipi", "Kpipipi"}) {
                            m_pars->AdjustValue(eff + "_efficiency_" + mode + run,
                                    pr->GetError(eff + run, mode));
                        }
                    }

                    // Bs selection efficiency
                    pr->ReadParameters("Bs_eff" + run, "Efficiencies/Values/total_efficiency_Bs"
                            + run + ".param");
                    m_pars->AdjustValue("tot_eff_Bs" + run, pr->GetError("Bs_eff" + run, "Kpi"));
                }
                break;
            }

        // Production asymmetry
        case (SysOption::production_asymmetry) :
            {
                for (std::string run : {"_run1", "_run2"}) {
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
                for (std::string run : {"_run1", "_run2"}) {
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
                for (std::string shape : {"Kpi", "Bs"}) {
                    std::string name = (shape == "Kpi") ? "signal" : shape;
                    pr->ReadParameters(name, "Fit_monte_carlo/Results/signal_" + shape + ".param");
                    for (std::string par : {"alpha_L", "alpha_R", "frac", "n_L", "n_R"}) {
                        m_pars->AdjustValue(name + "_" + par, pr->GetError(name, par));
                    }
                }
                break;
            }


        // Gamma and pi selection efficiencies
        case (SysOption::gamma_pi_selection) : 
            {
                pr->ReadParameters("acceptance", "Efficiencies/Values/acceptance_lowMass.param");
                for (std::string mode : {"gamma_010", "gamma_101", "pi_010", "pi_101"}) {
                    m_pars->AdjustValue(mode + "_acceptance", pr->GetError("acceptance", mode));
                }
                for (std::string run : {"_run1", "_run2"}) {
                    pr->ReadParameters("selection" + run, "Efficiencies/Values/selection_lowMass" 
                            + run + ".param");
                    for (std::string mode : {"gamma_010", "gamma_101", "pi_010", "pi_101"}) {
                        m_pars->AdjustValue(mode + "_selection" + run, 
                                pr->GetError("selection" + run, mode));
                    }
                }
                break;
            }

        // Gamma and pi branching ratios
        case (SysOption::gamma_pi_branching_ratios) :
            {
                pr->ReadParameters("BF", "Parameters/branching_fractions_lowMass.param");
                m_pars->AdjustValue("BF_gamma", pr->GetError("BF", "gamma"));
                m_pars->AdjustValue("BF_pi", pr->GetError("BF", "pi"));
                break;
            }

        // DKpipi inputs
        case (SysOption::DKpipi_inputs) :
            {
                pr->ReadParameters("DKpipi_obs", "Parameters/DKpipi_observables.param");
                m_pars->AdjustValue("A_DKpipi_KK", pr->GetError("DKpipi_obs", "A_KK") * 2);
                m_pars->AdjustValue("A_DKpipi_pipi", pr->GetError("DKpipi_obs", "A_pipi") * 2);
                m_pars->AdjustValue("R_DKpipi_GLW", pr->GetError("DKpipi_obs", "R_CP") * 2);
                m_pars->AdjustValue("A_DKpipi_pipipipi", pr->GetError("DKpipi_obs", "A_4pi") * 2);
                m_pars->AdjustValue("R_DKpipi_pipipipi", pr->GetError("DKpipi_obs", "R_4pi") * 2);
                break;
            }

        // None
        case (SysOption::none) :
            { 
                std::cout << "trying a test fit with no adjustment" << std::endl;
            }
    }
}
