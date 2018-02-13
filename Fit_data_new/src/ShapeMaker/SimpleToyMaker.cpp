#include "SimpleToyMaker.hpp"

// ===================
// Default constructor
// ===================
SimpleToyMaker::SimpleToyMaker() : SimpleShapeMakerBase("toy") {
}


// =====================
// Constructor with name
// =====================
SimpleToyMaker::SimpleToyMaker(std::string name) : SimpleShapeMakerBase(name) {
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

}
