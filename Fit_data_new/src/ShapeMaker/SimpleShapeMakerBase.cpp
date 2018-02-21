#include "SimpleShapeMakerBase.hpp"

// ===========
// Constructor
// ===========
SimpleShapeMakerBase::SimpleShapeMakerBase(std::string name, RooRealVar * x, 
        RooCategory * cat) : ShapeMakerBase(name, x, cat) {
}


// =======================
// Set constant parameters
// =======================
void SimpleShapeMakerBase::SetConstantParameters() {

    // Signal shape parameters
    m_pars->AddRealVar("mean", 5278.2);
    m_pars->AddRealVar("sigma_L", 20.67);
    m_pars->AddRealVar("alpha_L", 1.90);
    m_pars->AddRealVar("alpha_R", -3.00);
    m_pars->AddRealVar("n_L", 2.48);
    m_pars->AddRealVar("n_R", 0.76);
    m_pars->AddRealVar("ratio", 0.545);
    m_pars->AddRealVar("frac", 0.275);

    // Exponential slope
    m_pars->AddRealVar("slope", -0.00519);
}


// =========================================
// Set parameters with dependences on others
// =========================================
void SimpleShapeMakerBase::SetDependentParameters() {

    // RH crystal ball width: depends on LH width and ratio
    m_pars->AddFormulaVar("sigma_R", "@0 * @1", ParameterList("sigma_L", "ratio"));

    // piK signal yield: depends on Kpi yield and ratio
    m_pars->AddFormulaVar("n_signal_piK", "@0 * @1", ParameterList("n_signal_Kpi",
                "R_piK_vs_Kpi"));

}


// =========================
// Make each component shape
// =========================
void SimpleShapeMakerBase::MakeComponentShapes() {

    // Make double crystal ball shape for signal
    m_shapes->AddCrystalBall("CB_L", "mean", "sigma_L", "alpha_L", "n_L");
    m_shapes->AddCrystalBall("CB_R", "mean", "sigma_R", "alpha_R", "n_R");
    m_shapes->CombineShapes("signal", "CB_L", "CB_R", "frac");

    // Make exponential background shape
    m_shapes->AddExponential("expo", "slope");
}


// ==============================
// Make total shape for each mode
// ==============================
void SimpleShapeMakerBase::MakeModeShapes() {

    m_shapes->CombineShapes("Kpi", {{"signal", "n_signal_Kpi"}});
    m_shapes->CombineShapes("piK", {{"signal", "n_signal_piK"},
            {"expo", "n_expo_piK"}});
}
