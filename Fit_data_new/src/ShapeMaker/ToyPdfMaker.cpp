#include "ToyPdfMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
ToyPdfMaker::ToyPdfMaker(RooRealVar * x, RooCategory * cat, 
        std::string input_file) :
    DataPdfMaker("toy", x, cat, false),
    m_inputfile(input_file) {}

ToyPdfMaker::ToyPdfMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, std::string input_file) : 
    DataPdfMaker(name, x, cat, false),
    m_inputfile(input_file) {}


// ==========
// Destructor
// ==========
ToyPdfMaker::~ToyPdfMaker() {}


// =======================
// Set floating parameters
// =======================
void ToyPdfMaker::MakeComponents() {

    // Default function
    DataPdfMaker::MakeComponents();

    // Load in results for floating parameters from file
    m_pars->AddResultsFromFile(m_inputfile);

    // Use estimated values for blind parameters
    if (m_blind) {
        for (std::string sign : {"_plus", "_minus"}) {
            m_pars->ChangeValue("R_signal_piK" + sign, 0.06);
            m_pars->ChangeValue("R_signal_piKpipi" + sign, 0.06);
        }
        m_pars->ChangeValue("R_ds_pipipipi_run2", 0.147);
        m_pars->ChangeValue("R_signal_pipipipi_run2", 1);

        // Observables based on run
        for (std::string run : {"_run1", "_run2"}) {

            // Ratios
            m_pars->ChangeValue("R_signal_KK" + run, 1.054);
            m_pars->ChangeValue("R_signal_pipi" + run, 1.214);

            // Bs ratios
            m_pars->ChangeValue("R_ds_KK" + run, 0.103);
            m_pars->ChangeValue("R_ds_pipi" + run, 0.147);

            // Asymmetries
            m_pars->ChangeValue("A_signal_KK" + run, -0.198);
            m_pars->ChangeValue("A_signal_pipi" + run, -0.092);
            m_pars->ChangeValue("A_signal_pipipipi" + run, 0);
        }
    }

    // Turn warnings back on
    m_pars->SetWarnings(true);
}
