#include "ToyPdfMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
ToyPdfMaker::ToyPdfMaker(RooRealVar * x, RooCategory * cat, 
        std::string input_file) :
    DataPdfMaker("toy", x, cat),
    m_inputfile(input_file) {}

ToyPdfMaker::ToyPdfMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, std::string input_file) : 
    DataPdfMaker(name, x, cat),
    m_inputfile(input_file) {}


// ==========
// Destructor
// ==========
ToyPdfMaker::~ToyPdfMaker() {}


// =======================
// Set floating parameters
// =======================
void ToyPdfMaker::SetFloatingParameters() {

    // Make sure all floating parameters are set
    DataPdfMaker::SetFloatingParameters();
    m_pars->SetWarnings(false);

    // Load in results for floating parameters from file
    m_pars->AddResultsFromFile(m_inputfile);

    // Use estimated values for blind parameters
    // Ratios
    m_pars->AddRealVar("R_piK_vs_Kpi_signal", 0.06);
    m_pars->AddRealVar("R_KK_vs_Kpi_signal", 0.11);
    m_pars->AddRealVar("R_pipi_vs_Kpi_signal", 0.05);
    m_pars->AddRealVar("R_plus_signal", 0.06);
    m_pars->AddRealVar("R_minus_signal", 0.06);
    m_pars->AddRealVar("R_piKpipi_vs_Kpipipi_signal", 0.06);
    m_pars->AddRealVar("R_pipipipi_vs_Kpipipi_signal", 0.05);

    // Asymmetries
    m_pars->AddRealVar("A_KK_signal", -0.03);
    m_pars->AddRealVar("A_pipi_signal", -0.09);

    // Turn warnings back on
    m_pars->SetWarnings(true);
}
