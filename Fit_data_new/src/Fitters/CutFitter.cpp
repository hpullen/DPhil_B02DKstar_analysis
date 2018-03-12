#include <iostream>
#include <map>

#include "TFile.h"
#include "TTree.h"

#include "RooDataHist.h"

#include "CutFitter.hpp"

using namespace Data;


// ===========
// Constructor
// ===========
CutFitter::CutFitter() : 
    TwoAndFourBodyFitter(true),
    m_cuts(MakeDefaultCuts()) {}


// ==========
// Destructor
// ==========
CutFitter::~CutFitter() {}


// ===============================
// Set a single BDT cut for a mode
// ===============================
void CutFitter::SetCut(Mode mode, double cut) {
    std::vector<double> cut_vec = {cut};
    m_cuts[mode] = cut_vec;
}



// ============================
// Set range of cuts for a mode
// ============================
void CutFitter::SetCut(Mode mode, double min, double max, double increment) {
    if (min > max) {
        std::cout << "Error: minimum cut larger than maximum cut for " 
            << GetModeString(mode) << std::endl;
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

    // Make list of cuts
    std::vector<std::map<Mode, double>> cuts = MakeCutsList();
    for (auto cut : cuts) {

        // Set BDT cuts
        ResetArgs();
        for (auto mode : cut) {

            // Set BDT cuts in arg list
            std::string BDT_name = "BDTG_" + GetModeString(mode.first) + "_run2";
            AddArg(mode.first, BDT_name, mode.second, 1);
            if (mode.first == Mode::Kpi) {
                AddArg(Mode::piK, BDT_name, mode.second, 1);
            } else if (mode.first == Mode::Kpipipi) {
                AddArg(Mode::piKpipi, BDT_name, mode.second, 1);
            }

            // Fill tree branch values
            branches["cut_" + GetModeString(mode.first)] = mode.second;
        }

        // Get dataset
        RooDataHist * data = GetData();
        std::cout << "Got data. Entries: " << data->sumEntries() << std::endl;
        branches["entries"] = data->sumEntries();

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
    branches["cut_Kpi"] = 0;
    branches["cut_KK"] = 0;
    branches["cut_pipi"] = 0;
    branches["cut_Kpipipi"] = 0;
    branches["cut_pipipipi"] = 0;

    // Other
    branches["status"] = 0;
    branches["entries"] = 0;

    return branches;
}


// ====================================================
// Set up cuts map with default cut values for each BDT
// ====================================================
std::map<Mode, std::vector<double>> CutFitter::MakeDefaultCuts() {
    std::map<Mode, std::vector<double>> cuts;
    std::vector<double> default_vec = {0.5};
    cuts[Mode::Kpi] = default_vec;
    cuts[Mode::KK] = default_vec;
    cuts[Mode::pipi] = default_vec;
    cuts[Mode::Kpipipi] = default_vec;
    cuts[Mode::pipipipi] = default_vec;
    return cuts;
}


// ===================================================
// Convert map of cut lists to single list of cut maps
// ===================================================
std::vector<std::map<Mode, double>> CutFitter::MakeCutsList() {

    // Vector to hold each set of cuts
    std::vector<std::map<Mode, double>> cuts_list;
    std::map<Mode, double> cuts;

    // Fill the list with each combination of cuts
    for (auto cut_Kpi : m_cuts[Mode::Kpi]) {
        cuts[Mode::Kpi] = cut_Kpi;
        for (auto cut_KK : m_cuts[Mode::KK]) {
            cuts[Mode::KK] = cut_KK;
            for (auto cut_pipi : m_cuts[Mode::pipi]) {
                cuts[Mode::pipi] = cut_pipi;
                for (auto cut_Kpipipi : m_cuts[Mode::Kpipipi]) {
                    cuts[Mode::Kpipipi] = cut_Kpipipi;
                    for (auto cut_pipipipi : m_cuts[Mode::pipipipi]) {
                        cuts[Mode::pipipipi] = cut_pipipipi;
                        cuts_list.push_back(cuts);
                    }
                }
            }
        }
    }
    return cuts_list;
}
