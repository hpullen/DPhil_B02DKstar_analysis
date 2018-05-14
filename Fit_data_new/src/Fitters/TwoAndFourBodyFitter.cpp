#include "TwoAndFourBodyFitter.hpp"
#include "DataPdfMaker.hpp"

using namespace Data;

// ===========
// Constructor
// ===========
TwoAndFourBodyFitter::TwoAndFourBodyFitter(bool split, bool use_run2) : 
    DataFitter(new DataPdfMaker("pdf", MakeFitVariable(), 
                MakeCategory(split, use_run2), true), split) {}


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
    DataPdfMaker * data_pdf = (DataPdfMaker*)m_pdf;
    // data_pdf->PrintAsymmetries();
    // data_pdf->PrintRatios();
    // data_pdf->PrintYields();
    data_pdf->SaveHistograms(hist_file, data);
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
RooCategory * TwoAndFourBodyFitter::MakeCategory(bool split, bool use_run2) {

    // Make category
    RooCategory * cat = new RooCategory("category", "");

    // List of modes
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi"};
    // std::vector<std::string> modes = {"Kpi", "Kpipipi", "piK", "piKpipi", "KK"};
    if (use_run2) modes.push_back("pipipipi");

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
