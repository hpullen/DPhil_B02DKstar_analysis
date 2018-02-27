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
void TwoBodyFitter::AddArg(Mode mode, std::string arg_name, double min, double max) {
    DataFitter::AddArg(GetModeString(mode), arg_name, min, max);
}


// ===============
// Perform the fit
// ===============
void TwoBodyFitter::PerformFit(std::string results_file, std::string hist_file) {

    // Get the dataset
    RooDataHist * data = GetData();

    // Perform fit
    DataFitter::PerformFit(results_file, data);

    // Save histograms with blinding option
    TwoBodyPdfMaker * twoBody_pdf = (TwoBodyPdfMaker*)m_pdf;
    twoBody_pdf->SaveHistograms(hist_file, data);
}


// ==========================
// Create fit variable (Bd_M)
// ==========================
RooRealVar * TwoBodyFitter::MakeFitVariable() {
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
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
