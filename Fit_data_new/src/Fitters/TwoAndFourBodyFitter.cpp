#include "TwoAndFourBodyFitter.hpp"
#include "TwoAndFourBodyPdfMaker.hpp"

using namespace TwoAndFourBody;

// ===========
// Constructor
// ===========
TwoAndFourBodyFitter::TwoAndFourBodyFitter() : 
    DataFitter(new TwoAndFourBodyPdfMaker("pdf", MakeFitVariable(), MakeCategory(),
                true)) {}


// ==========
// Destructor
// ==========
TwoAndFourBodyFitter::~TwoAndFourBodyFitter() {}


// ==========
// Add a file
// ==========
void TwoAndFourBodyFitter::AddFile(Mode mode, std::string filepath) {
    DataFitter::AddFile(GetModeString(mode), filepath);
}


// ===============
// Add an argument
// ===============
void TwoAndFourBodyFitter::AddArg(Mode mode, std::string arg_name, double min, double max) {
    DataFitter::AddArg(GetModeString(mode), arg_name, min, max);
}


// ===============
// Perform the fit
// ===============
void TwoAndFourBodyFitter::PerformFit(std::string results_file, std::string hist_file) {

    // Get the dataset
    RooDataHist * data = GetData();

    // Perform fit
    DataFitter::PerformFit(results_file, data);

    // Save histograms with blinding option
    TwoAndFourBodyPdfMaker * twoAndFour_pdf = (TwoAndFourBodyPdfMaker*)m_pdf;
    twoAndFour_pdf->SaveHistograms(hist_file, data);
}


// ==========================
// Create fit variable (Bd_M)
// ==========================
RooRealVar * TwoAndFourBodyFitter::MakeFitVariable() {
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);
    return Bd_M;
}


// ===============
// Create category
// ===============
RooCategory * TwoAndFourBodyFitter::MakeCategory() {
    RooCategory * cat = new RooCategory("category", "");
    cat->defineType("Kpi");
    cat->defineType("piK");
    cat->defineType("KK");
    cat->defineType("pipi");
    cat->defineType("Kpipipi");
    cat->defineType("piKpipi");
    cat->defineType("pipipipi");
    return cat;
}
