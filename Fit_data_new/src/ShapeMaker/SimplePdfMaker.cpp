#include "SimplePdfMaker.hpp"

using namespace Simple;

// ============
// Constructors
// ============
SimplePdfMaker::SimplePdfMaker(RooRealVar * x, RooCategory * cat) : 
    SimpleShapeMakerBase("pdf", x, cat),
    m_hyp(Hypothesis::Signal) {
}

SimplePdfMaker::SimplePdfMaker(std::string name, RooRealVar * x, RooCategory * cat) :
    SimpleShapeMakerBase(name, x, cat),
    m_hyp(Hypothesis::Signal) {
}

SimplePdfMaker::SimplePdfMaker(RooRealVar * x, RooCategory * cat, Hypothesis hyp) : 
    SimpleShapeMakerBase("pdf", x, cat),
    m_hyp(hyp) {
}

SimplePdfMaker::SimplePdfMaker(std::string name, RooRealVar * x, RooCategory * cat,
        Hypothesis hyp) : 
    SimpleShapeMakerBase(name, x, cat),
    m_hyp(hyp) {
}


// ==========
// Destructor
// ==========
SimplePdfMaker::~SimplePdfMaker() {
}


// =======================
// Set floating parameters
// =======================
void SimplePdfMaker::SetFloatingParameters() {

    // Yields 
    m_pars->AddRealVar("n_signal_Kpi", 100, 0, 8000);
    m_pars->AddRealVar("n_expo_piK", 1000, 0, 8000);

    // Observables
    if(m_hyp == Hypothesis::Signal) {
        m_pars->AddRealVar("R_piK_vs_Kpi", 0.06, 0, 10);
    } else {
        m_pars->AddRealVar("R_piK_vs_Kpi", 0);
    }

}
