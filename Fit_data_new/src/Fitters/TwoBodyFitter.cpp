#include "TwoBodyFitter.hpp"
#include "TwoBodyPdfMaker.hpp"

using namespace TwoBody;

// ===========
// Constructor
// ===========
TwoBodyFitter::TwoBodyFitter() : 
    DataFitter(new TwoBodyPdfMaker("pdf", MakeFitVariable(), MakeCategory())) {}


// ==========
// Destructor
// ==========
TwoBodyFitter::~TwoBodyFitter() {}


// ==========
// Add a file
// ==========
void TwoBodyFitter::AddFile(Mode mode, std::string filepath) {
    DataFitter::AddFile(GetModeString(mode), filepath);
}


// ===============
// Add an argument
// ===============
void TwoBodyFitter::AddArg(Mode mode, std::string arg_name, double min, 
        double max) {
    DataFitter::AddArg(GetModeString(mode), arg_name, min, max);
}


// ===============
// Perform the fit
// ===============
void TwoBodyFitter::PerformFit(std::string file) {
    this->DataFitter::PerformFit(file);
}


// ==========================
// Create fit variable (Bd_M)
// ==========================
RooRealVar * TwoBodyFitter::MakeFitVariable() {
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);
    return Bd_M;
}


// ===============
// Create category
// ===============
RooCategory * TwoBodyFitter::MakeCategory() {
    RooCategory * cat = new RooCategory("category", "");
    cat->defineType("Kpi");
    cat->defineType("piK");
    cat->defineType("KK");
    cat->defineType("pipi");
    return cat;
}
