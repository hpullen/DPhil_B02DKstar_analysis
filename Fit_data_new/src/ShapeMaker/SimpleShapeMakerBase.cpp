#include "SimpleShapeMakerBase.hpp"

// ===========
// Constructor
// ===========
SimpleShapeMakerBase::SimpleShapeMakerBase(std::string name) : 
    ShapeMakerBase(name) {
}


// =======================
// Set constant parameters
// =======================
void SimpleShapeMakerBase::SetConstantParameters() {

    // Signal shape parameters
    RooRealVar * mean = new RooRealVar("mean", "", 5278.2);
    RooRealVar * sigma_L = new RooRealVar("sigma_L", "", 20.67);
    RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 1.90);
    RooRealVar * alpha_R = new RooRealVar("alpha_R", "", -3.00);
    RooRealVar * n_L = new RooRealVar("n_L", "", 2.48);
    RooRealVar * n_R = new RooRealVar("n_R", "", 0.76);
    RooRealVar * ratio = new RooRealVar("ratio", "", 0.545);
    RooRealVar * frac = new RooRealVar("frac", "", 0.275);

    // Exponential slope
    RooRealVar * slope = new RooRealVar("slope", "", -0.00519);
}


// =========================================
// Set parameters with dependences on others
// =========================================
void SimpleShapeMakerBase::SetDependentParameters() {

    // RH crystal ball width: depends on LH width and ratio
    RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", 
            RooArgList(*sigma_L, *ratio));
}


// =========================
// Make each component shape
// =========================
void SimpleShapeMakerBase::MakeComponentShapes() {
}


// ==============================
// Make total shape for each mode
// ==============================
void SimpleShapeMakerBase::MakeModeShapes() {

    // Make double crystal ball shape for signal
    RooCBShape * CB_L = new RooCBShape("CB_L", "", *Bd_M, 
            *mean, *sigma_L, *alpha_L, *n_L);
    RooCBShape * CB_R = new RooCBShape("CB_R", "", *Bd_M, 
            *mean, *sigma_R, *alpha_R, *n_R);
    RooAddPdf * signal_shape = new RooAddPdf("signal_shape", "",
            RooArgList(*CB_L, *CB_R), RooArgList(*frac));
}


// ====================
// Make RooSimultaneous
// ====================
void SimpleShapeMakerBase::MakeSimultaneousShape() {
}
