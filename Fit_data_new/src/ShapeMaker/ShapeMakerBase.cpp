#include "RooSimultaneous.h"

#include "ShapeMakerBase.hpp"
#include "ParameterManager.hpp"
#include "ShapeManager.hpp"

// ===========
// Constructor
// ===========
ShapeMakerBase::ShapeMakerBase(std::string name, RooRealVar * x) : 
    m_name(name) , m_x(x) {
    m_pars = new ParameterManager(name + "_params");
    m_shapes = new ShapeManager(name + "_shapes", m_x, m_pars);
}


// ==========
// Destructor
// ==========
ShapeMakerBase::~ShapeMakerBase() {
    delete m_pars;
}


// =========
// Get shape
// =========
RooSimultaneous * ShapeMakerBase::GetShape() {
    MakeShape();
    return m_pdf;
}


// =====================
// Make simultaneous PDF
// =====================
void ShapeMakerBase::MakeShape() {

    // Set up parameters
    SetConstantParameters();
    SetFloatingParameters();
    SetDependentParameters();

    // Set up shapes
    MakeComponentShapes();
    MakeModeShapes();
    MakeSimultaneousShape();

}
