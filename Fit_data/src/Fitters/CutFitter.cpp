#include <iostream>
#include <sstream>
#include <map>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"

#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"

#include "ParameterReader.hpp"
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
    while (current_cut <= max + 0.0000001) {
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

        // Get shape
        m_pdf->SetMaxYields(data);
        m_pdf->Shape();

        // Adjust efficiency if needed
        for (std::string mode : {"Kpi", "KK", "pipi", "Kpipipi", 
                "pipipipi"}) {
            if (cut[mode] != 0.5) {
                std::cout << "Cut is not 0.5 for " << mode << std::endl;
                for (std::string run : {"run1", "run2"}) {
                    if (mode == "pipipipi" && run == "run1") continue;
                    double old_eff = GetEfficiency(mode, run, 0.5);
                    double new_eff = GetEfficiency(mode, run, cut[mode]);
                    std::string par_name = "selection_efficiency_" + mode + 
                        "_" + run;
                    std::cout << "Old selection efficiency for " << run << ": "
                        << m_pdf->GetParameterValue(par_name) << std::endl;
                    m_pdf->SetParameter(par_name, m_pdf->GetParameterValue(par_name)
                            * new_eff / old_eff);
                    std::cout << "New selection efficiency for " << run << ": "
                        << m_pdf->GetParameterValue(par_name) << std::endl;
                }
            }
        }

        // Perform fit
        RooFitResult * result = m_pdf->Shape()->fitTo(*data, RooFit::Save(),
                RooFit::NumCPU(8, 2), RooFit::Optimize(false),
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2),
                RooFit::PrintLevel(-1));
        result->Print("v");

        // Save fit result for these cuts
        std::stringstream ss;
        ss << "/data/lhcb/users/pullen/B02DKstar/BDT_studies/data/Fit_results/data_fit";
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
        for (std::string run : {"_run1", "_run2"}) {
            for (std::string sign : {"_plus", "_minus"}) {

                // Integrate to get favoured mode signal
                auto S_Kpi = m_pdf->GetSignalIntegral("Kpi" + run + sign);
                branches["S_Kpi" + run + sign] = S_Kpi.first;
                branches["S_Kpi" + run + sign + "_err"] = S_Kpi.second;
                auto S_Kpipipi = m_pdf->GetSignalIntegral("Kpipipi" + run + sign);
                branches["S_Kpipipi" + run + sign] = S_Kpipipi.first;
                branches["S_Kpipipi" + run + sign + "_err"] = S_Kpipipi.second;

                // Read in predicted physics ratios from file
                ParameterReader * pr = new ParameterReader("../Parameters");
                pr->ReadParameters("obs", "predicted_observables.param");

                // Extrapolate suppressed/GLW mode signals
                // GLW: use correction factor
                for (std::string GLW : {"_KK", "_pipi"}) {
                    branches["S" + GLW + run + sign] = branches["S_Kpi" + run + sign] * 
                        pr->GetValue("obs", "R_CP") /
                        m_pdf->GetParameterValue("R_corr" + GLW + run);
                    branches["S" + GLW + run + sign + "_err"] = 
                        branches["S_Kpi" + run + sign + "_err"] * 
                        sqrt(branches["S" + GLW + run + sign]) /
                        sqrt(branches["S_Kpi" + run + sign]);
                }

                // ADS: use a_det differently for plus and minus
                if (sign == "_plus") {
                    branches["S_piK" + run + sign] = branches["S_Kpi" + run + sign] * 
                        pr->GetValue("obs", "R_plus") /
                        m_pdf->GetParameterValue("a_det_piK" + run);
                    branches["S_piKpipi" + run + sign] = branches["S_Kpipipi" + run + sign] * 
                        pr->GetValue("obs", "R_plus_K3pi") /
                        m_pdf->GetParameterValue("a_det_piKpipi" + run);
                } else {
                    branches["S_piK" + run + sign] = branches["S_Kpi" + run + sign] * 
                        pr->GetValue("obs", "R_plus") * 
                        m_pdf->GetParameterValue("a_det_piK" + run);
                    branches["S_piKpipi" + run + sign] = branches["S_Kpipipi" + run + sign] * 
                        pr->GetValue("obs", "R_plus_K3pi") * 
                        m_pdf->GetParameterValue("a_det_piKpipi" + run);
                }
                branches["S_piK" + run + sign + "_err"] =
                    branches["S_Kpi" + run + sign + "_err"] * 
                    sqrt(branches["S_piK" + run + sign]) /
                    sqrt(branches["S_Kpi" + run + sign]);
                branches["S_piKpipi" + run + sign + "_err"] =
                    branches["S_Kpipipi" + run + sign + "_err"] * 
                    sqrt(branches["S_piKpipi" + run + sign]) /
                    sqrt(branches["S_Kpipipi" + run + sign]);

                // 4pi: use correction factor
                if (run == "_run2") {
                    branches["S_pipipipi" + run + sign] = branches["S_Kpipipi" + 
                        run + sign] * pr->GetValue("obs", "R_CP_4pi") /
                        m_pdf->GetParameterValue("R_corr_pipipipi" + run);
                    branches["S_pipipipi" + run + sign + "_err"] = 
                        branches["S_Kpipipi" + run + sign + "_err"] * 
                        sqrt(branches["S_pipipipi" + run + sign]) /
                        sqrt(branches["S_Kpipipi" + run + sign]);
                }

                // Get background
                for (std::string mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", 
                        "piKpipi", "pipipipi"}) {
                    if (mode == "pipipipi" && run == "_run1") continue;
                    auto B_mode = m_pdf->GetBackgroundIntegral(mode + run + sign);
                    branches["B_" + mode + run + sign] = B_mode.first;
                    branches["B_" + mode + run + sign + "_err"] = B_mode.second;
                }
            }

            // Get significance for each run (summed over +/-)
            for (std::string mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", 
                    "piKpipi", "pipipipi"}) {
                if (mode == "pipipipi" && run == "_run1") continue;
                double S = branches["S_" + mode + run + "_plus"]
                    + branches["S_" + mode + run + "_minus"];
                double S_err = sqrt(pow(branches["S_" + mode + run + "_plus_err"], 2)
                        + pow(branches["S_" + mode + run + "_minus_err"], 2));
                double B = branches["B_" + mode + run + "_plus"]
                    + branches["B_" + mode + run + "_minus"];
                double B_err = sqrt(pow(branches["B_" + mode + run + "_plus_err"], 2)
                        + pow(branches["B_" + mode + run + "_minus_err"], 2));
                double sig = S/sqrt(S + B);
                branches["sig_" + mode + run] = sig;
                double dsig_dS = 1/sqrt(S + B) - 0.5 * S / ((S + B) * sqrt(S + B));
                double dsig_dB = -0.5 * S / ((S + B) * sqrt(S + B));
                double err = sqrt(pow(S_err * dsig_dS, 2) + pow(B_err * dsig_dB, 2)); 
                branches["sig_" + mode + run + "_err"] = err;
            }

        }

        // Save status
        branches["status"] = result->status();
        branches["covQual"] = result->covQual();

        // Fill tree
        tree->Fill();

        // If only using one cut, save histograms
        if (cuts.size() == 1) {
            std::cout << "SAVING HISTOGRAMS" << std::endl;
            m_pdf->SaveHistograms("Histograms/BDT_data.root", data, true);
        }
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
        for (std::string run : {"_run1", "_run2"}) {
            if (mode == "pipipipi" && run == "_run1") continue;
            for (std::string sign : {"plus", "minus"}) {
                branches["S_" + mode + run + "_" + sign] = 0;
                branches["B_" + mode + run + "_" + sign] = 0;
                branches["S_" + mode + run + "_" + sign + "_err"] = 0;
                branches["B_" + mode + run + "_" + sign + "_err"] = 0;
            }
            branches["sig_" + mode + run] = 0;
            branches["sig_" + mode + run + "_err"] = 0;
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


// ==========================================
// Read a single efficiency value from a file
// ==========================================
double CutFitter::GetEfficiency(std::string mode, std::string run, double cut) {

    // Name of variable
    std::stringstream ss;
    if (cut < -0.0001 || cut > 0.0001) {
        ss << "efficiency_" << std::setprecision(1) << cut;
    } else {
        ss << "efficiency_0.0";
    }

    // Directory and runs to include (just use 2012/2016 for simplicity)
    std::string eff_dir = "/home/pullen/analysis/B02DKstar/Optimize_BDT_cut/"
        "/MC_based/Efficiencies/";
    std::vector<std::string> mags = {"_up", "_down"};
    std::string year = (run == "run1") ? "2012" : "2016";

    // Loop through polarities and average
    double eff_sum = 0;
    for (auto mag : mags) {
        std::string filename = eff_dir + year + mag + "/signal_" + mode + ".root";
        TFile * file = TFile::Open(filename.c_str(), "READ");
        RooRealVar * eff = (RooRealVar*)file->Get(ss.str().c_str());
        eff_sum += eff->getVal();
        file->Close();
    }
    return eff_sum/2;
}
