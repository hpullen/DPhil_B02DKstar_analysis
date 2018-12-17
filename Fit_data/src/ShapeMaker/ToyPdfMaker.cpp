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

    // Read in predicted values for blind parameters
    ParameterReader * pr = new ParameterReader("../Parameters/");
    pr->ReadParameters("obs", "predicted_observables.param");

    // Use estimated values for blind parameters
    // ADS 
    m_pars->ChangeValue("R_signal_piK_plus", pr->GetValue("obs", "R_plus"));
    m_pars->ChangeValue("R_signal_piK_minus", pr->GetValue("obs", "R_minus"));
    m_pars->ChangeValue("R_signal_piK", pr->GetValue("obs", "R_ADS"));
    m_pars->ChangeValue("R_signal_piKpipi_plus", pr->GetValue("obs", "R_plus_K3pi"));
    m_pars->ChangeValue("R_signal_piKpipi_minus", pr->GetValue("obs", "R_minus_K3pi"));
    m_pars->ChangeValue("R_signal_piKpipi", pr->GetValue("obs", "R_ADS_K3pi"));

    // 4pi
    // m_pars->ChangeValue("R_ds_pipipipi_run2", 0.147);
    m_pars->ChangeValue("R_signal_pipipipi_run2", pr->GetValue("obs", "R_CP_4pi"));
    m_pars->ChangeValue("A_signal_pipipipi_run2", pr->GetValue("obs", "A_CP_4pi"));
    // m_pars->ChangeValue("R_ds_pipipipi_run2", pr->GetValue("obs", "R_ds_4pi"));

    // Observables based on run: GLW
    for (std::string run : {"_run1", "_run2"}) {

        // Ratios and asymmetries
        for (std::string mode : {"KK", "pipi"}) {
            m_pars->ChangeValue("R_signal_" + mode + run, pr->GetValue("obs", "R_CP"));
            m_pars->ChangeValue("A_signal_" + mode + run, pr->GetValue("obs", "A_CP"));
            // m_pars->ChangeValue("R_ds_" + mode + run, pr->GetValue("obs", "R_ds"));
        }
    }

    // Give the parameters their original uncertainties
    for (std::string run : {"_run1", "_run2"}) {
        for (std::string mode : {"KK", "pipi", "pipipipi"}) {
            if (mode == "pipipipi" && run == "_run1") continue;
            for (std::string par : {"R_signal_", "A_signal_"}) {
                if (m_pars->CheckForExistence(par + mode + run + "_blind")) {
                    m_pars->ChangeError(par + mode + run, m_pars->GetError(par + mode + run + "_blind"));
                }
            }
        }
    }
    for (std::string mode : {"piK", "piKpipi"}) {
        if (IsSplit()) {
            for (std::string sign : {"_plus", "_minus"}) {
                if (m_pars->CheckForExistence("R_signal_" + mode + sign + "_blind")) {
                    m_pars->ChangeError("R_signal_" + mode + sign, m_pars->GetError("R_signal_" + mode + sign + "_blind"));
                }
            }
        } else {
            if (m_pars->CheckForExistence("R_signal_" + mode + "_blind")) {
                m_pars->ChangeError("R_signal_" + mode, m_pars->GetError("R_signal_" + mode + "_blind"));
            }
        }
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


// ===========================
// Add extra events to a yield
// ===========================
void ToyPdfMaker::AddEvents(std::string name, double events) {
   RooRealVar * var = (RooRealVar*)m_pars->Get(name);
   m_pars->ChangeValue(name, var->getVal() + events, true);
}
