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
#include "DataPdfMaker.hpp"


// ===========
// Constructor
// ===========
CutFitter::CutFitter(DataPdfMaker * pdf) : 
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
        m_pdf->RemakeShape();

        // Perform the fit
        m_pdf->SetMaxYields(data);
        m_pdf->Shape();
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
        RooArgList pars = result->floatParsFinal();
        std::string par_string = pars.contentsString();
        for (auto branch : branches) {
            std::string br_noErr = branch.first.substr(0, branch.first.find("_err"));
            if (par_string.find(br_noErr) != std::string::npos) {
                std::string full_name = m_pdf->Name() + "_params_" + br_noErr;
                branches[branch.first] = 
                    ((RooRealVar*)pars.find(full_name.c_str()))->getError();
            }
        }

        // Fill significance estimates
        for (std::string sign : {"_plus", "_minus"}) {
            for (std::string run : {"_run1", "_run2"}) {
                branches["S_Kpi" + run + sign] = m_pdf->GetSignalIntegral("Kpi" + 
                        run + sign);
                branches["S_Kpipipi" + run + sign] = m_pdf->GetSignalIntegral("Kpipipi" 
                        + run + sign);
                branches["S_piK" + run + sign] = branches["S_Kpi" + run + sign] * 0.06;
                branches["S_KK" + run + sign] = branches["S_Kpi" + run + sign] * 0.11;
                branches["S_pipi" + run + sign] = branches["S_Kpi" + run + sign] * 0.05;
                branches["S_piKpipi" + run + sign] = branches["S_Kpipipi" + 
                    run + sign] * 0.06;
                if (run == "_run1") {
                    branches["S_pipipipi" + run + sign] = branches["S_Kpipipi" + 
                        run + sign] * 0.05;
                }
                for (std::string mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", 
                        "piKpipi", "pipipipi"}) {
                    if (mode == "pipipipi" && run == "_run1") continue;
                    double S = branches["S_" + mode + run + sign];
                    double B = m_pdf->GetBackgroundIntegral(mode + run + sign);
                    branches["B_" + mode + run + sign] = B;
                    branches["sig_" + mode + run + sign] = S / sqrt(S + B);
                }
            }
        }

        // Save status
        branches["status"] = result->status();
        branches["covQual"] = result->covQual();

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

    // Ratios
    for (std::string run : {"_run1", "_run2"}) {
        branches["R_signal_KK" + run + "_blind_err"] = 0;
        branches["R_signal_pipi" + run + "_blind_err"] = 0;
    }
    branches["R_signal_pipipipi_run2_blind_err"] = 0;

    // ADS ratios
    for (std::string ADS_mode : {"piK", "piKpipi"}) {
        for (std::string sign : {"plus", "minus"}) {
            branches["R_signal_" + ADS_mode + "_" + sign + "_blind_err"] = 0;
        }
    }

    // Asymmetries
    for (std::string asym_mode : {"KK", "pipi", "pipipipi"}) {
        for (std::string run : {"_run1", "_run2"}) {
            branches["A_signal_" + asym_mode + run + "_blind_err"] = 0;
        }
    }
    branches["A_signal_pipipipi_run2_blind_err"] = 0;

    // Yields in signal region
    for (std::string mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi",
            "pipipipi"}) {
        for (std::string sign : {"plus", "minus"}) {
            for (std::string run : {"_run1", "_run2"}) {
                if (mode == "pipipipi" && run == "_run1") continue;
                branches["S_" + mode + run + "_" + sign] = 0;
                branches["B_" + mode + run + "_" + sign] = 0;
                branches["sig_" + mode + run + "_" + sign] = 0;
            }
        }
    }

    // BDT cuts
    for (auto mode : m_cuts) {
        branches["cut_" + mode.first] = 0;
    }

    // Other
    branches["status"] = 0;
    branches["covQual"] = 0;

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
        std::string mode_short = mode.first.substr(0, mode.first.find("_run"));
        if (mode_short == "piK" || mode_short == "piKpipi") continue;
        std::string BDT_name = "BDTG_" + mode_short + "_run2";

        // Find loosest cut for the mode
        double min_cut = 1;
        for (auto cut_val : m_cuts[mode_short]) {
            if (cut_val < min_cut) min_cut = cut_val;
        }

        // Make variable
        BDT_vars[mode_short] = new RooRealVar(BDT_name.c_str(), "", min_cut, 1);
    }

    // Use same variable for ADS modes
    BDT_vars["piK"] = BDT_vars["Kpi"];
    BDT_vars["piKpipi"] = BDT_vars["Kpipipi"];

    // Loop through modes
    RooRealVar * KstarK_ID = new RooRealVar("KstarK_ID", "", -10000, 10000);
    for (auto mode : m_files) {

        std::string mode_short = mode.first.substr(0, mode.first.find("_run"));

        // Make args list
        RooArgList args;
        args.add(*m_pdf->FitVariable());
        args.add(*KstarK_ID);
        args.add(*BDT_vars[mode_short]);

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
        std::string mode_short = mode.first.substr(0, mode.first.find("_run"));
        for (std::string pol : {"plus", "minus"}) {
            std::string BDT_name = "BDTG_" + mode_short + "_run2";
            if (mode_short == "piK") BDT_name = "BDTG_Kpi_run2";
            else if (mode_short == "piKpipi") BDT_name = "BDTG_Kpipipi_run2";
            std::string cut = BDT_name + " > " + std::to_string(cuts[mode_short]);
            std::string cat_name = mode.first + "_" + pol;
            new_data_map[cat_name] =  (RooDataHist*)data_map[cat_name]->reduce(cut.c_str());
            std::cout << "Adding data to category " << cat_name << std::endl;
        }
    }

    // Combine data
    return new RooDataHist("combData", "", *m_pdf->FitVariable(), 
            *m_pdf->Category(), new_data_map);
}