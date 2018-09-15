#include "RunOneToyMaker.hpp"
#include "ParameterReader.hpp"

// ===========
// Constructor
// ===========
RunOneToyMaker::RunOneToyMaker(RooRealVar * x, RooCategory * cat, 
        std::string input_file) :
    DataPdfMaker("toy", x, cat),
    m_inputfile(input_file) {}

RunOneToyMaker::RunOneToyMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, std::string input_file) : 
    DataPdfMaker(name, x, cat),
    m_inputfile(input_file) {}


// ==========
// Destructor
// ==========
RunOneToyMaker::~RunOneToyMaker() {}


// =======================
// Set floating parameters
// =======================
void RunOneToyMaker::SetFloatingParameters() {

    // Make sure all floating parameters are set
    DataPdfMaker::SetFloatingParameters();
    m_pars->SetWarnings(false);

    // Load in results for floating parameters from file
    m_pars->AddResultsFromFile(m_inputfile);

    // Set yields etc. to Run 1 values
    // Yields
    m_pars->AddRealVar("n_signal_Kpi", 774.9);
    m_pars->AddRealVar("n_rho_Kpi", 52.7);
    m_pars->AddRealVar("n_Bs_piK", 1925.5);
    m_pars->AddRealVar("n_expo_Kpi", 1768.9);
    m_pars->AddRealVar("n_expo_piK", 922.6);
    m_pars->AddRealVar("n_expo_KK", 556.8);
    m_pars->AddRealVar("n_expo_pipi", 201.6);

    // Background vs. signal ratios
    m_pars->AddRealVar("BF_R_low_vs_signal", 1044.0/774.9);
    m_pars->AddRealVar("BF_R_Bs_low_vs_Bs", 2051.6/1925.5);
    m_pars->AddRealVar("BF_R_DKpipi_vs_signal", 0);

    // Suppressed vs. favoured ratios
    // Signal
    m_pars->AddRealVar("R_piK_vs_Kpi_signal", 0.06);
    m_pars->AddRealVar("R_KK_vs_Kpi_signal", 0.11);
    m_pars->AddRealVar("R_pipi_vs_Kpi_signal", 0.05);
    m_pars->AddRealVar("R_plus_signal", 0.06);
    m_pars->AddRealVar("R_minus_signal", 0.06);

    // Bs
    m_pars->AddRealVar("R_KK_vs_piK_Bs", 239.2/1925.5);
    m_pars->AddRealVar("R_pipi_vs_piK_Bs", 73.5/1925.5);

    // Low mass
    m_pars->AddRealVar("R_piK_vs_Kpi_low", 376.5/1044.0);
    m_pars->AddRealVar("R_KK_vs_Kpi_low", 111.4/1044.0);
    m_pars->AddRealVar("R_pipi_vs_Kpi_low", 52.7/1044.0);

    // Turn warnings back on
    m_pars->SetWarnings(true);
}
