#include "RooSimultaneous.h"

#include "ShapeMakerBase.hpp"
#include "ParameterManager.hpp"

// ===========
// Constructor
// ===========
ShapeMakerBase::ShapeMakerBase(std::string name) : m_name(name) {
    m_params = new ParameterManager(name + "_params");
}


// ==========
// Destructor
// ==========
ShapeMakerBase::~ShapeMakerBase() {
    delete m_params;
}


// =========
// Get shape
// =========
RooSimultaneousPdf * ShapeMakerBase::GetShape() {
    MakeShape();
    return m_shape;
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
