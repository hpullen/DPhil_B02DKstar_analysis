#include "SimpleToyMaker.hpp"

// ===================
// Default constructor
// ===================
SimpleToyMaker::SimpleToyMaker(RooRealVar * x, RooCategory * cat) : 
    SimpleShapeMakerBase("toy", x, cat) {
}


// =====================
// Constructor with name
// =====================
SimpleToyMaker::SimpleToyMaker(std::string name, RooRealVar * x, RooCategory * cat) : 
    SimpleShapeMakerBase(name, x, cat) {
}


// ==========
// Destructor
// ==========
SimpleToyMaker::~SimpleToyMaker() {
}


// ========================================================
// Set floating parameters to values from previous analysis
// ========================================================
void SimpleToyMaker::SetFloatingParameters() {

    // Yields 
    m_pars->AddRealVar("n_signal_Kpi", 774.9);
    m_pars->AddRealVar("n_expo_piK", 922.6);

    // Observables
    m_pars->AddRealVar("R_piK_vs_Kpi", 0.06);

}
