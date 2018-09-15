#include "TwoAndFourBodyToyMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
TwoAndFourBodyToyMaker::TwoAndFourBodyToyMaker(RooRealVar * x, RooCategory * cat, 
        std::string input_file) :
    TwoAndFourBodyBase("pdf", x, cat),
    m_inputfile(input_file) {}

TwoAndFourBodyToyMaker::TwoAndFourBodyToyMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, std::string input_file) : 
    TwoAndFourBodyBase(name, x, cat),
    m_inputfile(input_file) {}


// ==========
// Destructor
// ==========
TwoAndFourBodyToyMaker::~TwoAndFourBodyToyMaker() {}


// =======================
// Set floating parameters
// =======================
void TwoAndFourBodyToyMaker::SetFloatingParameters() {

    // Load in results for floating parameters from file
    m_pars->AddResultsFromFile(m_inputfile);

    // Use estimated values for blind parameters
    // Ratios
    m_pars->AddRealVar("R_piK_vs_Kpi", 0.06);
    m_pars->AddRealVar("R_KK_vs_Kpi", 0.11);
    m_pars->AddRealVar("R_pipi_vs_Kpi", 0.05);
    m_pars->AddRealVar("R_plus", 0.06);
    m_pars->AddRealVar("R_minus", 0.06);
    m_pars->AddRealVar("R_piKpipi_vs_Kpipipi", 0.06);
    m_pars->AddRealVar("R_pipipipi_vs_Kpipipi", 0.05);

    // Asymmetries
    m_pars->AddRealVar("A_KK", -0.03);
    m_pars->AddRealVar("A_pipi", -0.09);
}
