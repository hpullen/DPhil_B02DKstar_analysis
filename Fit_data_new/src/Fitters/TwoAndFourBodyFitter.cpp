#include "TwoAndFourBodyFitter.hpp"
#include "TwoAndFourBodyPdfMaker.hpp"

using namespace TwoAndFourBody;

// ===========
// Constructor
// ===========
TwoAndFourBodyFitter::TwoAndFourBodyFitter(bool split) : 
    DataFitter(new TwoAndFourBodyPdfMaker("pdf", MakeFitVariable(), 
                MakeCategory(split), true), split) {}


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
RooCategory * TwoAndFourBodyFitter::MakeCategory(bool split) {

    // Make category
    RooCategory * cat = new RooCategory("category", "");

    // List of modes
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi", "pipipipi"};

    // Loop through and add, splitting if requested
    for (auto mode : modes) {
        if (split) {
            cat->defineType((mode + "_plus").c_str());
            cat->defineType((mode + "_minus").c_str());
        } else {
            cat->defineType(mode.c_str());
        }
    }
    return cat;
}
