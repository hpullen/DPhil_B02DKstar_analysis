#include "TFile.h"
#include "TChain.h"
#include "TIterator.h"

#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooFitResult.h"
#include "RooRealVar.h"

#include "DataFitter.hpp"
#include "ShapeMakerBase.hpp"

// ===========
// Constructor
// ===========
DataFitter::DataFitter(ShapeMakerBase * shape, bool split) : 
    m_pdf(shape),
    m_ID(new RooRealVar("KstarK_ID", "", -100000, 100000)),
    m_split(split) {
    m_vars.emplace("Bd_M", shape->FitVariable());
}


// ==========
// Destructor
// ==========
DataFitter::~DataFitter() {}


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
    m_vars[arg_name] = new RooRealVar(arg_name.c_str(), "", min, max);
    m_args[mode]->add(*m_vars[arg_name]);
}


// ================
// Reset args lists
// ================
void DataFitter::ResetArgs() {
    for (auto args : m_args) {
        m_args[args.first] = new RooArgList();
    }
}


// ==================
// Print args in list
// ==================
void DataFitter::PrintArgs() {
    for (auto args : m_args) {
        std::cout << "Args for " << args.first << std::endl;
        TIterator * it = args.second->createIterator();
        RooRealVar * var;
        while ((var = (RooRealVar*)it->Next())) {
            var->Print();
        }
    }
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

        // Make sure KstarK ID is added to every variable if splitting the data
        if (m_split) {
            if (!m_args[mode.first]->contains(*m_ID)) {
                    m_args[mode.first]->add(*m_ID);
            }
        }

        // Fill chain
        TChain * chain = new TChain("DecayTree");
        for (auto file : mode.second) {
            chain->Add(file.c_str());
        }

        // Convert to RooDataHist
        RooDataSet * dataset = new RooDataSet(("data_" + mode.first).c_str(), "",
                chain, *m_args[mode.first]);
        if (m_split) {

            // Split by K*0 daughter kaon ID
            data_map[mode.first + "_plus"] = 
                ((RooDataSet*)dataset->reduce("KstarK_ID > 0"))->binnedClone();
            data_map[mode.first + "_minus"] = 
                ((RooDataSet*)dataset->reduce("KstarK_ID < 0"))->binnedClone();

        } else {
             data_map[mode.first] = dataset->binnedClone();
        }
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
    if (data->sumEntries() == 0) std::cout << "Warning: no data!" << std::endl;
    else m_pdf->SetMaxYields(data);

    // Fit and save RooFitResult
    RooFitResult * result = m_pdf->Shape()->fitTo(*data, RooFit::Save(),
            RooFit::NumCPU(8, 2), RooFit::Optimize(false), // RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    result->Print("v");
    TFile * results_file = new TFile(file.c_str(), "RECREATE");
    result->Write("fit_result");
    results_file->Close();
}
