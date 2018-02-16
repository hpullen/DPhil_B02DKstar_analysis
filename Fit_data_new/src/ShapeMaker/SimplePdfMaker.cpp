#include "SimplePdfMaker.hpp"

// ===================
// Default constructor
// ===================
SimplePdfMaker::SimplePdfMaker(RooRealVar * x, RooCategory * cat) : 
    SimpleShapeMakerBase("pdf", x, cat) {
}


// =====================
// Constructor with name
// =====================
SimplePdfMaker::SimplePdfMaker(std::string name, RooRealVar * x, RooCategory * cat) : 
    SimpleShapeMakerBase(name, x, cat) {
}


// ==========
// Destructor
// ==========
SimplePdfMaker::~SimplePdfMaker() {
}


// ========================================================
// Set floating parameters to values from previous analysis
// ========================================================
void SimplePdfMaker::SetFloatingParameters() {

    // Yields 
    m_pars->AddRealVar("n_signal_Kpi", 100, 0, 8000);
    m_pars->AddRealVar("n_expo_piK", 1000, 0, 8000);

    // Observables
    m_pars->AddRealVar("R_piK_vs_Kpi", 0.06, 0, 10);

}
