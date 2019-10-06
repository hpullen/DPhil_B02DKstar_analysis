#include <ctime>
#include <algorithm>

#include "TFile.h"
#include "TChain.h"
#include "TIterator.h"
#include "TStopwatch.h"

#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooAbsData.h"
#include "RooFitResult.h"
#include "RooRealVar.h"

#include "DataFitter.hpp"
#include "ShapeMakerBase.hpp"
#include "DataPdfMaker.hpp"

// ===========
// Constructor
// ===========
DataFitter::DataFitter(ShapeMakerBase * shape, bool split, bool split_obs) : 
    m_pdf(shape),
    m_split(split),
    m_split_obs(split_obs), 
    m_ID(new RooRealVar("KstarK_ID", "", -100000, 100000)) {
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
    if (std::find(m_modes.begin(), m_modes.end(), mode) == m_modes.end()) {
        m_modes.push_back(mode);
    }
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

    // Get map of unbinned dataset
    auto unbinned_data_map = GetUnbinnedDataMap();

    // Make binned map
    std::map<std::string, RooDataHist*> data_map;

    // Loop through and convert to binned dataset
    for (auto mode : unbinned_data_map) {
        data_map[mode.first] = mode.second->binnedClone();
    }

    // Make DataHist of all modes
    RooDataHist * data = new RooDataHist("combData", "", *m_pdf->FitVariable(),
            (*m_pdf->Category()), data_map);
    return data;
}


// ========================
// Get unbinned dataset map
// ========================
std::map<std::string, RooDataSet*> DataFitter::GetUnbinnedDataMap() {

    // Make map
    std::map<std::string, RooDataSet*> data_map;

    // Read in data from files
    RooRealVar * PID = new RooRealVar("KstarK_PIDK", "", 10, 100000); 
    for (auto mode : m_files) {

        // Make sure mass variable is added to every mode
        if (m_args.find(mode.first) == m_args.end()) {
            m_args.emplace(mode.first, new RooArgList());
        }
        if (!m_args[mode.first]->contains(*m_pdf->FitVariable())) {
            m_args[mode.first]->add(*m_pdf->FitVariable());
        }
        m_args[mode.first]->add(*PID);

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

        // Make dataset
        RooDataSet * dataset = new RooDataSet(("data_" + mode.first).c_str(), "",
                chain, *m_args[mode.first]);
        if (m_split) {
            // Split by K*0 daughter kaon ID
            data_map[mode.first + "_plus"] = (RooDataSet*)dataset->reduce("KstarK_ID > 0");
            data_map[mode.first + "_minus"] = (RooDataSet*)dataset->reduce("KstarK_ID < 0");
            std::cout << data_map[mode.first + "_plus"]->sumEntries() << " entries in " << 
                mode.first << " plus." << std::endl;

        } else {
             data_map[mode.first] = dataset;
        }
    }
    return data_map;
}


// ====================
// Get unbinned dataset
// ====================
RooDataSet * DataFitter::GetUnbinnedData() {

    // Get map
    auto data_map = GetUnbinnedDataMap();

    // Make dataset
    RooDataSet * data = new RooDataSet("combData", "", *m_pdf->FitVariable(),
            RooFit::Index(*m_pdf->Category()), RooFit::Import(data_map));
    std::cout << "Combined dataset has " << data->sumEntries() << " entries." 
        << std::endl;
    return data;
}

// =============================
// Perform the fit to given data
// =============================
RooFitResult * DataFitter::PerformFit(std::string file, RooAbsData * data, 
        bool save_extras) {

    // Adjust maximum yields to match dataset
    if (data->sumEntries() == 0) std::cout << "Warning: no data!" << std::endl;
    else m_pdf->SetMaxYields(data);

    // Fit and save RooFitResult
    m_pdf->Shape();
    // m_pdf->PrintToFile("data_pdf_before_fit.txt");
    RooFitResult * result;
    TStopwatch * sw = new TStopwatch();
    if (m_pdf->HasConstraints()) {
        result = m_pdf->Shape()->fitTo(*data, RooFit::Save(),
                RooFit::ExternalConstraints(*m_pdf->GetConstraintPdfs()),
                RooFit::NumCPU(8), RooFit::Optimize(false), RooFit::Offset(true),
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    } else {
        std::cout << "Performing extended fit" << std::endl;
        result = m_pdf->Shape()->fitTo(*data, RooFit::Save(), RooFit::Extended(kTRUE),
                RooFit::NumCPU(4), RooFit::Optimize(false), RooFit::Offset(false),
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    }
    std::cout << "Time elapsed: " << sw->CpuTime() << std::endl;
    // m_pdf->PrintToFile("data_pdf_after_fit.txt");
    result->Print("v");
    TFile * results_file = new TFile(file.c_str(), "RECREATE");
    result->Write("fit_result");

    // Write derived parameters to a workspace in file
    if (save_extras) {

        // Check which runs are present
        std::vector<std::string> runs = {""};
        for (auto mode : m_modes) {
            if (mode.find("_run") != std::string::npos) {
                runs = {"_run1", "_run2"};
                break;
            }
        }
        std::vector<std::string> obs_runs = {""};
        if (m_split_obs) obs_runs = runs;
        RooWorkspace * wspace = new RooWorkspace();

        // Save Bs-related GLW ratios
        for (std::string mode : {"KK", "pipi", "pipipipi"}) {
            for (std::string run : runs) {
                if (mode == "pipipipi" && run == "_run1") continue;
                RooFormulaVar * R_ds = (RooFormulaVar*)m_pdf->GetParameter("R_ds_" + mode + run);
                wspace->import(*R_ds, RooFit::RecycleConflictNodes());
                RooFormulaVar * R_Bs = (RooFormulaVar*)m_pdf->GetParameter("R_Bs_" + mode + run);
                wspace->import(*R_Bs, RooFit::RecycleConflictNodes());
            }
        }

        // Save R_ADS and A_ADS
        if (m_split) {
            for (std::string mode : {"piK", "piKpipi"}) {
                for (std::string run : obs_runs) {
                    RooFormulaVar * R_ADS = (RooFormulaVar*)m_pdf->GetParameter("R_ADS_" + mode + run);
                    wspace->import(*R_ADS, RooFit::RecycleConflictNodes());
                    RooFormulaVar * A_ADS = (RooFormulaVar*)m_pdf->GetParameter("A_ADS_" + mode + run);
                    wspace->import(*A_ADS, RooFit::RecycleConflictNodes());
                }
            }
        }

        // Save
        wspace->Write("wspace");
    }

    results_file->Close();
    return result;
}
