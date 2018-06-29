#include "TwoAndFourBodyFitter.hpp"
#include "DataPdfMaker.hpp"

using namespace Data;

// ===========
// Constructor
// ===========
TwoAndFourBodyFitter::TwoAndFourBodyFitter(bool split, Data::Run run_opt) : 
    DataFitter(new DataPdfMaker("pdf", MakeFitVariable(), 
                MakeCategory(split, run_opt), true), split) {}


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
 

// ==========================================
// Add a file for Run 1 or Run 2 specifically
// ==========================================
void TwoAndFourBodyFitter::AddFile(Mode mode, int run, std::string filepath) {
    DataFitter::AddFile(GetModeString(mode) + "_run" + std::to_string(run), 
            filepath);
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
    RooFitResult * r = DataFitter::PerformFit(results_file, data);

    // Save histograms with blinding option
    DataPdfMaker * data_pdf = (DataPdfMaker*)m_pdf;
    // data_pdf->PrintAsymmetries();
    // data_pdf->PrintRatios();
    data_pdf->PrintYields(r);
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
RooCategory * TwoAndFourBodyFitter::MakeCategory(bool split, Data::Run run_opt) {

    // Make category
    RooCategory * cat = new RooCategory("category", "");

    // List of modes
    // std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        // "piKpipi"};
    std::vector<std::string> modes = {"Kpi", "piK", "Kpipipi"};
    // if (run_opt == Data::Run::Both || run_opt == Data::Run::Run2) {
        // modes.push_back("pipipipi");
    // }

    // Get vector of runs
    std::vector<std::string> runs;
    if (run_opt == Data::Run::Both) {
        runs.push_back("_run1");
        runs.push_back("_run2");
    } else {
        runs.push_back("");
    }

    // Loop through and add, splitting if requested
    for (auto mode : modes) {
        for (auto run : runs) {
            if (mode == "pipipipi" && run == "_run1") continue;
            if (split) {
                cat->defineType((mode + run + "_plus").c_str());
                cat->defineType((mode + run + "_minus").c_str());
            } else {
                cat->defineType((mode + run).c_str());
            }
        }
    }
    return cat;
}
