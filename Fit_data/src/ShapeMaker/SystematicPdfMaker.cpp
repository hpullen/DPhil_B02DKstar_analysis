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
            pr->ReadParameters("fs_fd", "Parameters/hadronization_fraction.param");
            m_pars->AdjustValue("fs_fd", pr->GetError("fs_fd", "fs_fd"), true);
            break;

        // Helicity fraction for piK
        case (SysOption::low_frac_piK) :
            gRandom->SetSeed(0);
            double random = gRandom->Rndm();
            std::cout << "Changing to " << random << std::endl;
            m_pars->ChangeValue("low_frac_010_piK", random);
            m_pars->SetConstant("low_frac_010_piK");
            break;
    }
}
