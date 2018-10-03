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
                    for (std::string eff : {"acceptance", "selection"}) {
                        pr->ReadParameters(eff + run, "Efficiencies/Values/" + eff + "_efficiency"
                                + run + ".param");
                        for (std::string mode : {"Kpi", "KK", "pipi", "pipipipi", "Kpipipi"}) {
                            m_pars->AdjustValue(eff + "_efficiency_" + mode + run,
                                    pr->GetError(eff + run, mode));
                        }
                    }
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

            }

        // None
        case (SysOption::none) :
            { 
                std::cout << "trying a test fit with no adjustment" << std::endl;
            }
    }
}
