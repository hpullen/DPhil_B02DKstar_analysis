#include "ToyPdfMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
ToyPdfMaker::ToyPdfMaker(RooRealVar * x, RooCategory * cat, 
        std::string input_file) :
    DataPdfMaker("toy", x, cat, false),
    m_inputfile(input_file),
    m_high_stats(false) {}

ToyPdfMaker::ToyPdfMaker(RooRealVar * x, RooCategory * cat, 
        std::string input_file, bool high_stats) :
    DataPdfMaker("toy", x, cat, false),
    m_inputfile(input_file),
    m_high_stats(high_stats) {}

ToyPdfMaker::ToyPdfMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, std::string input_file) : 
    DataPdfMaker(name, x, cat, false),
    m_inputfile(input_file),
    m_high_stats(false) {}


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
    // ADS 
    m_pars->ChangeValue("R_signal_piK_plus", 0.052);
    m_pars->ChangeValue("R_signal_piK_minus", 0.065);
    m_pars->ChangeValue("R_signal_piKpipi_plus", 0.061);
    m_pars->ChangeValue("R_signal_piKpipi_minus", 0.047);

    // 4pi
    // m_pars->ChangeValue("R_ds_pipipipi_run2", 0.147);
    m_pars->ChangeValue("R_signal_pipipipi_run2", 0.99);
    m_pars->ChangeValue("A_signal_pipipipi_run2", -0.026);

    // Observables based on run: GLW
    for (std::string run : {"_run1", "_run2"}) {

        // Ratios and asymmetries
        for (std::string mode : {"KK", "pipi"}) {
            m_pars->ChangeValue("R_signal_" + mode + run, 0.93);
            m_pars->ChangeValue("A_signal_" + mode + run, -0.049);
        }

        // Bs ratios
        // m_pars->ChangeValue("R_ds_KK" + run, 0.103);
        // m_pars->ChangeValue("R_ds_pipi" + run, 0.147);
    }

    // Turn warnings back on
    m_pars->SetWarnings(true);

    // Set high stats if requested
    if (m_high_stats) AdjustYields();

}


// =======================
// Turn on high stats mode
// =======================
void ToyPdfMaker::SetHighStats(bool high_stats) {
    m_high_stats = high_stats;
}


// =======================
// Multiply yields by 1000
// =======================
void ToyPdfMaker::AdjustYields() {

     double scale = 2;
     std::cout << "Adjusting yields by " << scale << std::endl;

     for (std::string run : {"_run1", "_run2"}) {

         // Signal/rho
         for (std::string mode : {"Kpi", "Kpipipi"}) {
             AdjustYield("N_signal_" + mode + run, scale);
         }

         // Bs
         for (std::string mode : {"piK", "piKpipi"}) {
             AdjustYield("N_Bs_" + mode + run, scale);
         }

         // Combinatorial
         for (std::string mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi",
                 "piKpipi", "pipipipi"}) {
             AdjustYield("N_expo_" + mode + run, scale);
         }
     }
}


// =========================================
// Multiply a single yield by a scale factor
// =========================================
void ToyPdfMaker::AdjustYield(std::string name, double scale) {
    RooRealVar * var = (RooRealVar*)m_pars->Get(name);
    m_pars->ChangeValue(name, var->getVal() * scale, var->getMin() * scale,
            var->getMax() * scale);
}
