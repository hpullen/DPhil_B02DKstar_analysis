#include <iostream>
#include <fstream>
#include <map>

#include "TCanvas.h"
#include "TChain.h"
#include "TCut.h"
#include "TPad.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TIterator.h"
#include "TStyle.h"

#include "RooAbsPdf.h"
#include "RooArgList.h"
#include "RooFitResult.h"
#include "RooGaussian.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "RooWorkspace.h"

#include "PlotStyle.hpp"

int main (int argc, char * argv[]) {

    // Set plotting style
    setPlotStyle();

    // Name of toy set
    if (argc != 2 && argc != 3) {
        std::cout << "Usage: ./PlotSystematics <name>" << std::endl;
        return -1;
    }
    std::string set_name = argv[1];
    std::string dir = "";
    bool combined = false;
    bool split_obs = false;
    if (argc > 2) {
        std::string opt = argv[2];
        if (opt == "--combineRuns") {
            dir = "CombineRuns/";
            combined = true;
        } else if (opt == "--splitObs") {
            dir = "/SplitObs/";
            split_obs = true;
        } else {
            std::cout << "Unrecognised option: " << opt << std::endl;
        }
    }

    // List of observables
    std::vector<std::string> obs = {
        "A_signal_Kpi",
        "A_signal_Kpipipi",
        "A_Bs_piK",
        "A_Bs_piKpipi",
        "R_signal_piK_plus",
        "R_signal_piK_minus",
        "R_signal_piKpipi_plus",
        "R_signal_piKpipi_minus",
    };
    if (split_obs) {
        obs = {
        "A_signal_Kpi_run1",
        "A_signal_Kpipipi_run1",
        "A_Bs_piK_run1",
        "A_Bs_piKpipi_run1",
        "R_signal_piK_run1_plus",
        "R_signal_piK_run1_minus",
        "R_signal_piKpipi_run1_plus",
        "R_signal_piKpipi_run1_minus",
        "A_signal_Kpi_run2",
        "A_signal_Kpipipi_run2",
        "A_Bs_piK_run2",
        "A_Bs_piKpipi_run2",
        "R_signal_piK_run2_plus",
        "R_signal_piK_run2_minus",
        "R_signal_piKpipi_run2_plus",
        "R_signal_piKpipi_run2_minus"
        };
    }
    std::vector<std::string> GLW_obs;
    if (split_obs) {
        GLW_obs = {
            "A_signal_KK_run1",
            "A_signal_KK_run2",
            "A_signal_pipi_run1",
            "A_signal_pipi_run2",
            "A_Bs_KK_run1",
            "A_Bs_KK_run2",
            "A_Bs_pipi_run1",
            "A_Bs_pipi_run2",
            "R_Bs_KK_run1",
            "R_Bs_KK_run2",
            "R_Bs_pipi_run1",
            "R_Bs_pipi_run2",
            "R_signal_KK_run1",
            "R_signal_KK_run2",
            "R_signal_pipi_run2",
            "R_signal_pipi_run1",
            "R_signal_pipipipi_run2",
            "A_Bs_pipipipi_run2",
            "A_signal_pipipipi_run2",
            "R_Bs_pipipipi_run2"
        };
    } else {
        GLW_obs = {
            "A_signal_KK",
            "A_signal_pipi",
            "A_signal_pipipipi",
            "A_Bs_KK",
            "A_Bs_pipi",
            "A_Bs_pipipipi",
            "R_Bs_KK_run1",
            "R_Bs_KK_run2",
            "R_Bs_pipi_run1",
            "R_Bs_pipi_run2",
            "R_Bs_pipipipi_run2",
            "R_signal_KK",
            "R_signal_pipi",
            "R_signal_pipipipi",
            "R_ADS_piK",
            "A_ADS_piK",
            "R_ADS_piKpipi",
            "A_ADS_piKpipi"
        };
    }
    obs.insert(obs.end(), GLW_obs.begin(), GLW_obs.end());

    // Store R_Bs for averaging
    std::map<std::string, double> R_Bs_stat;
    std::map<std::string, double> R_Bs_sys;

    // Read in toy files
    TString tree_name = (set_name == "charmless") ? "toy_tree" : "sys_tree";
    TChain * sys_tree = new TChain(tree_name);
    sys_tree->Add(("/data/lhcb/users/pullen/B02DKstar/systematics/" + dir + set_name + "/*.root").c_str());
    std::cout << "Loaded systematics tree with " << sys_tree->GetEntries() << " entries." 
        << std::endl;

    // Extra tree for charmless
    TChain * toy_tree;
    if (set_name == "charmless") {
        toy_tree = new TChain("toy_tree");
        toy_tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/FitterBias/Binned/split/" + dir + "*.root").c_str());
        std::cout << "Loaded toy tree with " << toy_tree->GetEntries() << " entries." 
            << std::endl;
    }

    // Map to hold means of Gaussians
    std::map<std::string, std::pair<double, double>> width_map;
    std::map<std::string, std::pair<double, double>> width_map_all;

    // Read in original fit result (for stat uncertainty)
    TString res_filename = "../Fit_data/Results/twoAndFourBody_data_split.root";
    if (combined) res_filename = "../Fit_data/Results/twoAndFourBody_data_split_combinedRuns.root";
    if (split_obs) res_filename = "../Fit_data/Results/twoAndFourBody_data_split_splitObs.root";
    TFile * res_file = TFile::Open(res_filename, "READ");
    RooFitResult * result = (RooFitResult*)res_file->Get("fit_result");
    RooWorkspace * wspace = (RooWorkspace*)res_file->Get("wspace");
    RooArgList args = result->floatParsFinal();

    // Loop through variables and plot spread of each
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    for (auto var : obs) {

        // See if it's R_Bs
        bool is_R_Bs = false;
        if (var.find("R_Bs_") == 0) is_R_Bs = true;

        // Extra cut to help with fitting
        TCut extra_cut = "";
        if (set_name == "background_shape_pars") { 
           if (var == "R_signal_pipi") {
                extra_cut = "R_signal_pipi > 1.32";
           } else if (var == "R_signal_pipipipi") {
                extra_cut = "R_signal_pipipipi > 1.01";
           } else if (var == "A_signal_Kpipipi") {
                extra_cut = "A_signal_Kpipipi < 0.037";
           } else if (var == "R_signal_KK") {
               extra_cut = "R_signal_KK > 0.9";
           } else if (var == "A_signal_Kpi_run1") {
               extra_cut = "A_signal_Kpi_run1 < 0.0385";
           } else if (var == "A_signal_Kpipipi_run1") {
               extra_cut = "A_signal_Kpipipi_run1 < 0.0281";
           } else if (var == "R_signal_KK_run1") {
               extra_cut = "R_signal_KK_run1 > 0.9";
           } else if (var == "R_signal_pipi_run1") {
               extra_cut = "R_signal_pipi_run1 > 1.39";
           } else if (var == "R_signal_pipipipi_run2") {
               extra_cut = "R_signal_pipipipi_run2 > 1";
           }
        } else if (set_name == "rho_PID") {
            if (var == "R_signal_piK_minus") {
                extra_cut = "R_signal_piK_minus > 0.0936 && R_signal_piK_minus < 0.0944";
            }
        }

        // Make histogram
        if (set_name != "charmless") {
            sys_tree->Draw((var + ">>hist_" + var).c_str(), "status == 0 && covQual == 3" + extra_cut);
        } else {
            sys_tree->Draw(("sys_signal_final_value_" + var + ">>hist_" + var).c_str(), "status == 0 && covQual == 3");
        }
        TH1F * sys_hist = (TH1F*)gDirectory->Get(("hist_" + var).c_str());

        // Plot histograms
        gStyle->SetFrameBorderSize(1);
        gStyle->SetCanvasBorderSize(1);
        gStyle->SetOptStat(0);
        gStyle->SetOptFit(0001);
        gStyle->SetStatW(0.15);
        gStyle->SetStatH(0.2);
        gStyle->SetStatX(0.85);
        gStyle->SetStatColor(1);
        gStyle->SetStatBorderSize(1);
        sys_hist->SetLineWidth(1);
        canvas->cd();
        sys_hist->Draw("E");

        // Fit the pull histogram with a Gaussian
        sys_hist->Fit("gaus");
        TF1 * gauss_fit = sys_hist->GetFunction("gaus");
        gauss_fit->SetLineColor(kBlue);
        gauss_fit->SetLineWidth(2);
        double sys = gauss_fit->GetParameter("Sigma");
        double mean = gauss_fit->GetParameter("Mean");

        // Draw
        gauss_fit->Draw("C SAME");
        sys_hist->Draw("E SAME");

        // If charmless, compare with normal toy distribution
        if (set_name == "charmless") {
            toy_tree->Draw(("signal_final_value_" + var + ">>toy_hist_" + var).c_str(), "status == 0 && covQual == 3");
            TH1F * toy_hist = (TH1F*)gDirectory->Get(("toy_hist_" + var).c_str());
            toy_hist->SetLineColor(kRed);
            toy_hist->Fit("gaus");
            TF1 * gauss_fit_toy = toy_hist->GetFunction("gaus");
            gauss_fit_toy->SetLineColor(kRed);
            gauss_fit_toy->SetLineWidth(2);
            toy_hist->Draw("E SAME");
            gauss_fit_toy->Draw("C SAME");
            gauss_fit->Draw("C SAME");
            sys_hist->Draw("E SAME");
            sys = std::fabs(mean - gauss_fit_toy->GetParameter("Mean"));
        }

        // Save the canvas
        canvas->SaveAs(("Plots/" + dir + set_name + "/" + var + ".pdf").c_str());

        // Use RMS for production asymmetry
        // if (set_name == "production_asymmetry") {
            // sys = sys_hist->GetRMS();
        // }

        // Add to map if not 2 order of magnitude smaller than stat
        double stat;
        if (var.find("R_ds_") != std::string::npos || var.find("R_Bs_") != std::string::npos 
                || var.find("_ADS_") != std::string::npos) {
            RooFormulaVar * fvar = (RooFormulaVar*)wspace->arg(("pdf_params_" + var).c_str());
            stat = fvar->getPropagatedError(*result);
        } else {
            stat = ((RooRealVar*)args.find(("pdf_params_" + var).c_str()))->getError();
        }
        std::cout << "Stat uncertainty: " << stat << std::endl;
        std::cout << "Sys uncertainty: " << sys << std::endl;
        std::string var_to_store = var;
        if (log10(stat) - log10(sys) < 2) {
            width_map[var] = std::make_pair(sys, stat);
            if (!split_obs && var == "R_Bs_pipipipi_run2") {
                width_map["R_Bs_pipipipi"] = std::make_pair(sys, stat);
            }
        }
        width_map_all[var] = std::make_pair(sys, stat);
        if (!split_obs && var == "R_Bs_pipipipi_run2") {
            width_map_all["R_Bs_pipipipi"] = std::make_pair(sys, stat);
        }

        // Store in R_Bs map
        if (is_R_Bs && !split_obs) {
            R_Bs_stat[var] = stat;
            R_Bs_sys[var] = sys;
        }
        
    } // End loop over fit parameters

    // Average R_Bs 
    if (!split_obs) {

        // Average for KK and pipi
        double frac_1 = 1530.0/(1530 + 2200);
        double frac_2 = 2200.0/(1530 + 2200);
        for (std::string mode : {"KK", "pipi"}) {
            double stat_1 = R_Bs_stat["R_Bs_" + mode + "_run1"];
            double stat_2 = R_Bs_stat["R_Bs_" + mode + "_run2"];
            double stat = sqrt(pow(stat_1 * frac_1, 2) + pow(stat_2 * frac_2, 2));
            double sys_1 = R_Bs_sys["R_Bs_" + mode + "_run1"];
            double sys_2 = R_Bs_sys["R_Bs_" + mode + "_run2"];
            double sys = sqrt(pow(sys_1 * frac_1, 2) + pow(sys_2 * frac_2, 2));
            width_map_all["R_Bs_" + mode] = std::make_pair(stat, sys);
            if (log10(stat) - log10(sys) < 2) {
                width_map["R_Bs_" + mode] = std::make_pair(sys, stat);
            }
        }
    }

    // Print systematic uncertainty results to file
    std::ofstream file("Results/" + dir + set_name + ".param");
    for (auto width : width_map) {
        file << width.first << " " << width.second.first << " ("
            << width.second.second << ")" << std::endl;
    }
    file.close();
    std::ofstream file_all("Results/all/" + dir + set_name + ".param");
    for (auto width : width_map_all) {
        file_all << width.first << " " << width.second.first << " ("
            << width.second.second << ")" << std::endl;
    }
    file_all.close();

    return 0;
}
