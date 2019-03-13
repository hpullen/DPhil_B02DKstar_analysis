#include "ToyPdfMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
ToyPdfMaker::ToyPdfMaker(RooRealVar * x, RooCategory * cat, 
        std::string input_file, bool split_obs) :
    DataPdfMaker("toy", x, cat, false, split_obs),
    m_inputfile(input_file),
    m_high_stats(false),
    m_combine_runs(false),
    m_split_obs(split_obs) {
    }

ToyPdfMaker::ToyPdfMaker(RooRealVar * x, RooCategory * cat, 
        std::string input_file, bool high_stats, bool combine_runs,
        bool split_obs) :
    DataPdfMaker("toy", x, cat, false, split_obs),
    m_inputfile(input_file),
    m_high_stats(high_stats), 
    m_combine_runs(combine_runs),
    m_split_obs(split_obs) 
{
}

ToyPdfMaker::ToyPdfMaker(std::string name, RooRealVar * x, 
        RooCategory * cat, std::string input_file, bool split_obs) : 
    DataPdfMaker(name, x, cat, false, split_obs),
    m_inputfile(input_file),
    m_high_stats(false),
    m_combine_runs(false),
    m_split_obs(split_obs)
{
}


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

    // // Read in predicted values for blind parameters
    // ParameterReader * pr = new ParameterReader("../Parameters/");
    // pr->ReadParameters("obs", "predicted_observables.param");

    // // Use estimated values for blind parameters
    // // ADS
    // std::vector<std::string> runs = {""};
    // if (!m_combine_runs) runs = {"_run1", "_run2"};
    // for (std::string run : runs) {
        // m_pars->ChangeValue("R_signal_piK_plus" + run, pr->GetValue("obs", "R_plus"));
        // m_pars->ChangeValue("R_signal_piK_minus" + run, pr->GetValue("obs", "R_minus"));
        // m_pars->ChangeValue("R_signal_piK" + run, pr->GetValue("obs", "R_ADS"));
        // m_pars->ChangeValue("R_signal_piKpipi_plus" + run, pr->GetValue("obs", "R_plus_K3pi"));
        // m_pars->ChangeValue("R_signal_piKpipi_minus" + run, pr->GetValue("obs", "R_minus_K3pi"));
        // m_pars->ChangeValue("R_signal_piKpipi" + run, pr->GetValue("obs", "R_ADS_K3pi"));
    // }

    // // 4pi
    // m_pars->ChangeValue("R_signal_pipipipi_run2", pr->GetValue("obs", "R_CP_4pi"));
    // m_pars->ChangeValue("A_signal_pipipipi_run2", pr->GetValue("obs", "A_CP_4pi"));

    // // Observables based on run: GLW
    // for (std::string run : runs) {
        // // Ratios and asymmetries
        // for (std::string mode : {"KK", "pipi"}) {
            // if (run == "_run1") {
                // m_pars->ChangeValue("R_signal_" + mode + run, pr->GetValue("obs", "R_CP"));
            // } else {
                // m_pars->ChangeValue("R_signal_" + mode + run, pr->GetValue("obs", "R_CP"));
            // }
            // m_pars->ChangeValue("A_signal_" + mode + run, pr->GetValue("obs", "A_CP"));
        // }
    // }

    // // Give the parameters their original uncertainties
    // for (std::string run : runs) {
        // for (std::string mode : {"KK", "pipi", "pipipipi"}) {
            // if (mode == "pipipipi" && run == "_run1") continue;
            // for (std::string par : {"R_signal_", "A_signal_"}) {
                // if (m_pars->CheckForExistence(par + mode + run + "_blind")) {
                    // m_pars->ChangeError(par + mode + run, m_pars->GetError(par + mode + run + "_blind"));
                // }
            // }
        // }
    // }
    // for (std::string mode : {"piK", "piKpipi"}) {
        // if (IsSplit()) {
            // for (std::string sign : {"_plus", "_minus"}) {
                // if (m_pars->CheckForExistence("R_signal_" + mode + sign + "_blind")) {
                    // m_pars->ChangeError("R_signal_" + mode + sign, m_pars->GetError("R_signal_" + mode + sign + "_blind"));
                // }
            // }
        // } else {
            // if (m_pars->CheckForExistence("R_signal_" + mode + "_blind")) {
                // m_pars->ChangeError("R_signal_" + mode, m_pars->GetError("R_signal_" + mode + "_blind"));
            // }
        // }
    // }

    // // Turn warnings back on
    // m_pars->SetWarnings(true);

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

     double scale = 10;
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
