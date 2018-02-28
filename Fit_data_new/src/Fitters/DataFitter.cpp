#include "TFile.h"
#include "TChain.h"

#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooFitResult.h"
#include "RooRealVar.h"

#include "DataFitter.hpp"
#include "ShapeMakerBase.hpp"

// ===========
// Constructor
// ===========
DataFitter::DataFitter(ShapeMakerBase * shape) : m_pdf(shape) {
    m_vars.emplace("Bd_M", shape->FitVariable());
}


// ====================
// Fit to data with PDF
// ====================
void DataFitter::PerformFit(std::string file) {
    PerformFit(file, GetData());
}


// ==================
// Add a file to list
// ==================
void DataFitter::AddFile(std::string mode, std::string filepath) {
    m_files[mode].push_back(filepath);
}


// ===============================
// Add an argument (cut) to a mode
// ===============================
void DataFitter::AddArg(std::string mode, std::string arg_name, double min,
        double max) {
    if (m_args.find(mode) == m_args.end()) {
        m_args.emplace(mode, new RooArgList());
    }
    m_vars.emplace(arg_name, new RooRealVar(arg_name.c_str(), "", min, max));
    m_args[mode]->add(*m_vars[arg_name]);
}


// ===============================
// Open data files and get dataset
// ===============================
RooDataHist * DataFitter::GetData() {

    // Map to hold data hists
    std::map<std::string, RooDataHist*> data_map;

    // Read in data from files
    for (auto mode : m_files) {

        // Make sure mass variable is added to every mode
        if (!m_args[mode.first]->contains(*m_pdf->FitVariable())) {
            m_args[mode.first]->add(*m_pdf->FitVariable());
        }

        // Fill chain
        TChain * chain = new TChain("DecayTree");
        for (auto file : mode.second) {
            chain->Add(file.c_str());
        }

        // Convert to RooDataHist
        RooDataSet * dataset = new RooDataSet(("data_" + mode.first).c_str(), "",
                chain, *m_args[mode.first]);
         data_map[mode.first] = dataset->binnedClone();
    }

    // Make DataHist of all modes
    RooDataHist * data = new RooDataHist("combData", "", *m_pdf->FitVariable(),
            *m_pdf->Category(), data_map);
    return data;
}


// =============================
// Perform the fit to given data
// =============================
void DataFitter::PerformFit(std::string file, RooDataHist * data) {

    // Adjust maximum yields to match dataset
    m_pdf->SetMaxYields(data);
    m_pdf->RemakeShape();

    // Fit and save RooFitResult
    RooFitResult * result = m_pdf->Shape()->fitTo(*data, RooFit::Save(),
            RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true), 
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    result->Print("v");
    TFile * results_file = new TFile(file.c_str(), "RECREATE");
    result->Write("fit_result");
    results_file->Close();
}
