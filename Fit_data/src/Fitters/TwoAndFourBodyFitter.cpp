#include "RooFitResult.h"
#include "TwoAndFourBodyFitter.hpp"
#include "DataPdfMaker.hpp"

using namespace Data;

// ===========
// Constructor
// ===========
TwoAndFourBodyFitter::TwoAndFourBodyFitter(bool split, Data::Run run_opt, std::vector<std::string> modes, bool split_obs) : 
    DataFitter(new DataPdfMaker("pdf", MakeFitVariable(), MakeCategory(split, run_opt, modes), false, split_obs), split, split_obs),
    m_split_obs(split_obs) {}


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
double TwoAndFourBodyFitter::PerformFit(std::string results_file, std::string hist_file, bool binned) {

    // Get the dataset
    RooAbsData * data;
    if (binned) {
        data = GetData();
    } else {
        data = GetUnbinnedData();
    }

    // Perform fit
    RooFitResult * r = DataFitter::PerformFit(results_file, data, true);
    std::cout << "NLL: " << r->minNll() << std::endl;

    // Save histograms with blinding option
    DataPdfMaker * data_pdf = (DataPdfMaker*)m_pdf;
    data_pdf->PrintYields(r);
    data_pdf->SaveHistograms(hist_file, data, binned);

    // Return NLL
    return r->minNll();
}


// ==================================
// Perform fit with a parameter fixed
// ==================================
double TwoAndFourBodyFitter::PerformFixedParFit(std::string par, double val, std::string results_file, 
       std::string hist_file, bool binned) {

    // Fit
    m_pdf->RemakeShape();
    DataPdfMaker * data_pdf = (DataPdfMaker*)m_pdf;
    data_pdf->SetFixedPar(par, val);
    double nll = PerformFit(results_file, hist_file, binned);
    std::cout << "NLL: " << nll << std::endl;
    return nll;

}

// ================================
// Perform fit with null hypothesis
// ================================
double TwoAndFourBodyFitter::PerformSignificanceFit(std::string mode, std::string results_file, std::string hist_file, bool binned) {

    // Perform normal fit
    double nll = PerformFit(results_file, hist_file, binned);

    // Make null filenames
    size_t res_pos = results_file.find(".root");
    std::string null_results_file = results_file.substr(0, res_pos) + "_" + mode + "_null.root";
    size_t hist_pos = hist_file.find(".root");
    std::string null_hist_file = hist_file.substr(0, hist_pos) + "_" + mode + "_null.root";

    // Perform null fit
    m_pdf->RemakeShape();
    DataPdfMaker * data_pdf = (DataPdfMaker*)m_pdf;
    data_pdf->SetZeroYield(mode);
    double nll_null = PerformFit(null_results_file, null_hist_file, binned);

    // Get significance
    std::cout << "NLL: " << nll << std::endl;
    std::cout << "Null NLL: " << nll_null << std::endl;
    return sqrt(2 * (nll_null - nll));
}


// ==========================
// Create fit variable (Bd_M)
// ==========================
RooRealVar * TwoAndFourBodyFitter::MakeFitVariable() {
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    return Bd_M;
}


// ===============
// Create category
// ===============
RooCategory * TwoAndFourBodyFitter::MakeCategory(bool split, Data::Run run_opt, std::vector<std::string> modes) {

    // Make category
    RooCategory * cat = new RooCategory("category", "");

    // Loop through and add, splitting if requested
    for (auto const & mode : modes) {
        if (run_opt == Data::Run::Both) {
            for (std::string const & run : {"_run1", "_run2"}) {
                if (mode == "pipipipi" && run == "_run1") continue;
                if (split) {
                    cat->defineType((mode + run + "_plus").c_str());
                    cat->defineType((mode + run + "_minus").c_str());
                } else {
                    cat->defineType((mode + run).c_str());
                }
            }
        } else {
            if (split) {
                cat->defineType((mode + "_plus").c_str());
                cat->defineType((mode + "_minus").c_str());
            } else {
                cat->defineType((mode).c_str());
            }
        }
    }
    return cat;
}
