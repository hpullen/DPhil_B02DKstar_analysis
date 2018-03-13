#include <iostream>
#include <sstream>
#include <map>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"

#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"

#include "CutFitter.hpp"
#include "ShapeMakerBase.hpp"


// ===========
// Constructor
// ===========
CutFitter::CutFitter(ShapeMakerBase * pdf) : 
    m_cuts(MakeDefaultCuts()),
    m_pdf(pdf) {}


// ==========
// Destructor
// ==========
CutFitter::~CutFitter() {}


// =============
// Add data file
// =============
void CutFitter::AddFile(std::string mode, std::string filepath) {
   m_files[mode].push_back(filepath); 
}


// ===============================
// Set a single BDT cut for a mode
// ===============================
void CutFitter::SetCut(std::string mode, double cut) {
    std::vector<double> cut_vec = {cut};
    m_cuts[mode] = cut_vec;
}



// ============================
// Set range of cuts for a mode
// ============================
void CutFitter::SetCut(std::string mode, double min, double max, double increment) {
    if (min > max) {
        std::cout << "Error: minimum cut larger than maximum cut for " << mode 
            << std::endl;
        exit (EXIT_FAILURE);
    }
    std::vector<double> cut_vec;
    double current_cut = min;
    while (current_cut <= max) {
        cut_vec.push_back(current_cut);
        current_cut += increment;
    }
    m_cuts[mode] = cut_vec;
}


// =================
// Perform the study
// =================
void CutFitter::PerformStudy(std::string filename) {

    // Open file and make tree
    TFile * file = TFile::Open(filename.c_str(), "RECREATE");
    TTree * tree = new TTree("study_tree", "");

    // Set up tree branches
    std::map<std::string, double> branches = GetBranches();
    for (auto branch : branches) {
        tree->Branch(branch.first.c_str(), &branches[branch.first], 
                (branch.first + "/D").c_str());
    }

    // Settings
    RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING);

    // Get full dataset with loosest cuts applied
    std::map<std::string, RooDataHist*> full_data = GetFullDataset();
    for (auto mode : full_data) {
        std::cout << mode.first << " entries: " << mode.second->sumEntries() << std::endl;
    }

    // Make list of cuts and loop over
    std::vector<std::map<std::string, double>> cuts = MakeCutsList();
    for (auto cut : cuts) {

        // Fill BDT cuts
        for (auto mode : cut) {
            branches["cut_" + mode.first] = mode.second;
        }

        // Reduce dataset with current cuts
        RooDataHist * data = GetCutDataset(full_data, cut);
        m_pdf->SetMaxYields(data);
        std::cout << "Reduce dataset to " << data->sumEntries() << " entries." 
            << std::endl;

        // Perform the fit
        std::cout << "Performing fit with cuts:";
        for (auto mode : cut) {
            std::cout << " " << mode.first << ">" << mode.second;
        } 
        std::cout << std::endl;
        RooFitResult * result = m_pdf->Shape()->fitTo(*data, RooFit::Save(),
                RooFit::NumCPU(8, 2), RooFit::Optimize(false),
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2),
                RooFit::PrintLevel(-1));
        result->Print("v");

        // Save fit result for these cuts
        std::stringstream ss;
        ss << "Results/BDT/Fit_results/data_fit";
        for (auto mode : cut) {
            ss << "_" << mode.first << "_" << mode.second;
        }
        ss << ".root";
        TFile * res_file = TFile::Open(ss.str().c_str(), "RECREATE");
        result->Write("fit_result");
        res_file->Close();

        // Fill errors

        // Fill tree
        tree->Fill();
    }

    // Write tree
    file->cd();
    tree->Write();
    file->Close();
}


// ====================
// Set branches in tree
// ====================
std::map<std::string, double> CutFitter::GetBranches() {

    // Make map of desired branches
    std::map<std::string, double> branches;

    // Ratio errors
    branches["R_plus_signal_err"] = 0;
    branches["R_minus_signal_err"] = 0;
    branches["R_KK_vs_Kpi_signal_err"] = 0;
    branches["R_pipi_vs_Kpi_signal_err"] = 0;

    // Asymmetry errors
    branches["A_KK_signal_err"] = 0;
    branches["A_pipi_signal_err"] = 0;

    // Yields in signal region

    // BDT cuts
    for (auto mode : m_cuts) {
        branches["cut_" + mode.first] = 0;
    }

    // Other
    branches["status"] = 0;
    branches["entries"] = 0;

    return branches;
}


// ====================================================
// Set up cuts map with default cut values for each BDT
// ====================================================
std::map<std::string, std::vector<double>> CutFitter::MakeDefaultCuts() {
    std::map<std::string, std::vector<double>> cuts;
    std::vector<double> default_vec = {0.5};
    cuts["Kpi"] = default_vec;
    cuts["KK"] = default_vec;
    cuts["pipi"] = default_vec;
    cuts["Kpipipi"] = default_vec;
    cuts["pipipipi"] = default_vec;
    return cuts;
}


// ===================================================
// Convert map of cut lists to single list of cut maps
// ===================================================
std::vector<std::map<std::string, double>> CutFitter::MakeCutsList() {

    // Vector to hold each set of cuts
    std::vector<std::map<std::string, double>> cuts_list;
    std::map<std::string, double> cuts;

    // Fill the list with each combination of cuts
    for (auto cut_Kpi : m_cuts["Kpi"]) {
        cuts["Kpi"] = cut_Kpi;
        for (auto cut_KK : m_cuts["KK"]) {
            cuts["KK"] = cut_KK;
            for (auto cut_pipi : m_cuts["pipi"]) {
                cuts["pipi"] = cut_pipi;
                for (auto cut_Kpipipi : m_cuts["Kpipipi"]) {
                    cuts["Kpipipi"] = cut_Kpipipi;
                    for (auto cut_pipipipi : m_cuts["pipipipi"]) {
                        cuts["pipipipi"] = cut_pipipipi;
                        cuts_list.push_back(cuts);
                    }
                }
            }
        }
    }
    return cuts_list;
}


// ==============================================
// Get the full dataset with minimum cuts applied
// ==============================================
std::map<std::string, RooDataHist*> CutFitter::GetFullDataset() {

    // Create map to hold data
    std::map<std::string, RooDataHist*> data_map;

    // Make map of BDT variables
    std::map<std::string, RooRealVar*> BDT_vars;
    for (auto mode : m_files) {

        // Name of BDT
        if (mode.first == "piK" || mode.first == "piKpipi") continue;
        std::string BDT_name = "BDTG_" + mode.first + "_run2";

        // Find loosest cut for the mode
        double min_cut = 1;
        for (auto cut_val : m_cuts[mode.first]) {
            if (cut_val < min_cut) min_cut = cut_val;
        }

        // Make variable
        BDT_vars[mode.first] = new RooRealVar(BDT_name.c_str(), "", min_cut, 1);
    }

    // Use same variable for ADS modes
    BDT_vars["piK"] = BDT_vars["Kpi"];
    BDT_vars["piKpipi"] = BDT_vars["Kpipipi"];

    // Loop through modes
    RooRealVar * KstarK_ID = new RooRealVar("KstarK_ID", "", -10000, 10000);
    for (auto mode : m_files) {


        // Make args list
        RooArgList args;
        args.add(*m_pdf->FitVariable());
        args.add(*KstarK_ID);
        args.add(*BDT_vars[mode.first]);

        // Fill chain
        TChain * chain = new TChain("DecayTree");
        for (auto file : mode.second) {
            chain->Add(file.c_str());
        }

        // Convert to RooDataHist
        RooDataSet * data = new RooDataSet(("data_" + mode.first).c_str(), "",
                chain, args);
        data_map[mode.first + "_plus"] = ((RooDataSet*)data->reduce("KstarK_ID > 0"))->binnedClone();
        data_map[mode.first + "_minus"] = ((RooDataSet*)data->reduce("KstarK_ID < 0"))->binnedClone();
    }

    return data_map;
}


// =========================
// Reduce dataset using cuts
// =========================
RooDataHist * CutFitter::GetCutDataset(std::map<std::string, RooDataHist*> data_map,
        std::map<std::string, double> cuts) {

    // Map for reduced datasets
    std::map<std::string, RooDataHist*> new_data_map;

    // Use same cut for ADS modes
    cuts["piK"] = cuts["Kpi"];
    cuts["piKpipi"] = cuts ["Kpipipi"];

    // Apply each cut to the dataset
    for (auto mode : m_files) {
        std::cout << "Mode: " << mode.first << std::endl;
        for (std::string pol : {"plus", "minus"}) {
            std::string BDT_name = "BDTG_" + mode.first + "_run2";
            if (mode.first == "piK") BDT_name = "BDTG_Kpi_run2";
            else if (mode.first == "piKpipi") BDT_name = "BDTG_Kpipipi_run2";
            std::string cut = BDT_name + " > " + std::to_string(cuts[mode.first]);
            std::string cat_name = mode.first + "_" + pol;
            new_data_map[cat_name] =  (RooDataHist*)data_map[cat_name]->reduce(cut.c_str());
        }
    }

    // Combine data
    return new RooDataHist("combData", "", *m_pdf->FitVariable(), 
            *m_pdf->Category(), new_data_map);
}
