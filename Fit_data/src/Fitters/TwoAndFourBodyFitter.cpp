#include "TwoAndFourBodyFitter.hpp"
#include "DataPdfMaker.hpp"

using namespace Data;

// ===========
// Constructor
// ===========
TwoAndFourBodyFitter::TwoAndFourBodyFitter(bool split, Data::Run run_opt, std::vector<std::string> modes, 
        bool split_GLW) : 
    DataFitter(new DataPdfMaker("pdf", MakeFitVariable(), MakeCategory(split, run_opt, modes), 
                true, split_GLW), split),
    m_split_GLW(split_GLW) {}


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
void TwoAndFourBodyFitter::PerformFit(std::string results_file, std::string hist_file, bool binned, bool save_weights) {

    // Get the dataset
    RooAbsData * data;
    if (binned) {
        data = GetData();
    } else {
        data = GetUnbinnedData();
    }

    // Perform fit
    RooFitResult * r = DataFitter::PerformFit(results_file, data, m_split_GLW);

    // Save histograms with blinding option
    DataPdfMaker * data_pdf = (DataPdfMaker*)m_pdf;
    data_pdf->PrintYields(r);
    data_pdf->SaveHistograms(hist_file, data, binned);

    // // Save weights if requested
    // if (save_weights) {

        // // Make vectors of files to save to
        // std::vector<std::string> Kpi_files_run1;
        // std::vector<std::string> Kpi_files_run2;
        // std::vector<std::string> Kpipipi_files_run1;
        // std::vector<std::string> Kpipipi_files_run2;
        // std::string dir = "/data/lhcb/users/pullen/B02DKstar/data/";
        // for (std::string mag : {"_up", "_down"}) {
            // for (std::string year : {"2011", "2012"}) {
                // Kpi_files_run1.push_back(dir + "twoBody/" + year + mag + "/Kpi_selected.root");
                // Kpipipi_files_run1.push_back(dir + "fourBody/" + year + mag + "/Kpipipi_selected.root");
            // }
            // for (std::string year : {"2015", "2016"}) {
                // Kpi_files_run2.push_back(dir + "twoBody/" + year + mag + "/Kpi_selected.root");
                // Kpipipi_files_run2.push_back(dir + "fourBody/" + year + mag + "/Kpipipi_selected.root");
            // }
        // }

        // // Get data to use
        // RooDataSet * data_to_use;
        // if (binned) {
            // data_to_use = GetUnbinnedData();
        // } else {
            // data_to_use = (RooDataSet*)data;
        // }

        // // Save weights
        // data_pdf->ApplyWeights("Kpi_run1", data_to_use, Kpi_files_run1, r);
        // data_pdf->ApplyWeights("Kpi_run2", data_to_use, Kpi_files_run2, r);
        // data_pdf->ApplyWeights("Kpipipi_run1", data_to_use, Kpipipi_files_run1, r);
        // data_pdf->ApplyWeights("Kpipipi_run2", data_to_use, Kpipipi_files_run2, r);
    // }
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

// ===============================
// Separate R± for run 1 and run 2
// ===============================
void TwoAndFourBodyFitter::SeparateRruns() {
    ((DataPdfMaker*)m_pdf)->SeparateRruns();
}
