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
    bool combined = false;
    if (argc > 2) {
        std::string opt = argv[2];
        if (opt == "--combineRuns") {
            set_name = "CombinedRuns/" + set_name;
            combined = true;
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
        "R_signal_piK_plus_blind",
        "R_signal_piK_minus_blind",
        "R_signal_piKpipi_plus_blind",
        "R_signal_piKpipi_minus_blind",
    };
    std::vector<std::string> GLW_obs;
    if (!combined) {
        GLW_obs = {
            "A_signal_KK_run1_blind",
            "A_signal_KK_run2_blind",
            "A_signal_pipi_run1_blind",
            "A_signal_pipi_run2_blind",
            "A_Bs_KK_run1",
            "A_Bs_KK_run2",
            "A_Bs_pipi_run1",
            "A_Bs_pipi_run2",
            "R_ds_KK_run1_blind",
            "R_ds_KK_run2_blind",
            "R_ds_pipi_run1_blind",
            "R_ds_pipi_run2_blind",
            "R_signal_KK_run1_blind",
            "R_signal_KK_run2_blind",
            "R_signal_pipi_run2_blind",
            "R_signal_pipi_run1_blind",
            "R_signal_pipipipi_run2_blind",
            "A_Bs_pipipipi_run2",
            "A_signal_pipipipi_run2_blind",
            "R_ds_pipipipi_run2_blind"
        };
    } else {
        GLW_obs = {
            "A_signal_KK_blind",
            "A_signal_pipi_blind",
            "A_Bs_KK",
            "A_Bs_pipi",
            "R_ds_KK_blind",
            "R_ds_pipi_blind",
            "R_signal_KK_blind",
            "R_signal_pipi_blind"
        };
    }
    obs.insert(obs.end(), GLW_obs.begin(), GLW_obs.end());

    // Only use R_ds if considering fs/fd (not used in fit)
    if (set_name == "fs_fd") {
        if (!combined) {
            obs = {"R_ds_KK_run1_blind",
                "R_ds_KK_run2_blind",
                "R_ds_pipi_run1_blind",
                "R_ds_pipi_run2_blind",
                "R_ds_pipipipi_run2_blind"};
        } else {
            obs = {"R_ds_KK_blind",
                "R_ds_pipi_blins"};
        }
    }

    // Read in toy files
    TString tree_name = (set_name == "charmless") ? "toy_tree" : "sys_tree";
    TChain * sys_tree = new TChain(tree_name);
    sys_tree->Add(("/data/lhcb/users/pullen/B02DKstar/systematics/" + set_name + "/*.root").c_str());
    std::cout << "Loaded systematics tree with " << sys_tree->GetEntries() << " entries." 
        << std::endl;

    // Extra tree for charmless
    TChain * toy_tree;
    if (set_name == "charmless") {
        toy_tree = new TChain("toy_tree");
        toy_tree->Add("/data/lhcb/users/pullen/B02DKstar/toys/FitterBias/Binned/*.root");
    }

    // Map to hold means of Gaussians
    std::map<std::string, std::pair<double, double>> width_map;
    std::map<std::string, std::pair<double, double>> width_map_all;

    // Read in original fit result (for stat uncertainty)
    TString res_filename = "../Fit_data/Results/twoAndFourBody_data_split.root";
    if (combined) res_filename = "../Fit_data/Results/twoAndFourBody_data_split_combinedRuns.root";
    TFile * res_file = TFile::Open(res_filename, "READ");
    RooFitResult * result = (RooFitResult*)res_file->Get("fit_result");
    RooWorkspace * wspace = (RooWorkspace*)res_file->Get("wspace");
    RooArgList args = result->floatParsFinal();

    // Loop through variables and plot spread of each
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    for (auto var : obs) {

        // Shorten variable if needed
        std::string var_short = var;
        if (set_name == "charmless") {
            var_short = var.substr(0, var.find("_blind"));
        }

        // Extra cut to help with fitting
        TCut extra_cut = "";
        if (set_name == "background_shape_pars") { 
           if (var == "R_signal_pipi_run1_blind") {
                extra_cut = "R_signal_pipi_run1_blind > 1.355";
           } else if (var == "R_signal_KK_run1_blind") {
                extra_cut = "R_signal_KK_run1_blind > 0.9";
           }
        }

        // Make histogram
        if (set_name != "charmless") {
            sys_tree->Draw((var + ">>hist_" + var).c_str(), "status == 0 && covQual == 3" + extra_cut);
        } else {
            sys_tree->Draw(("sys_signal_final_value_" + var_short + ">>hist_" + var).c_str(), "status == 0 && covQual == 3");
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
            toy_tree->Draw(("signal_final_value_" + var_short + ">>toy_hist_" + var).c_str(), "status == 0 && covQual == 3");
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
        canvas->SaveAs(("Plots/" + set_name + "/" + var + ".pdf").c_str());

        // Use RMS for production asymmetry
        // if (set_name == "production_asymmetry") {
            // sys = sys_hist->GetRMS();
        // }

        // Add to map if not 2 order of magnitude smaller than stat
        double stat;
        if (var.find("R_ds_") != std::string::npos) {
            RooFormulaVar * fvar = (RooFormulaVar*)wspace->arg(("pdf_params_" + var).c_str());
            stat = fvar->getPropagatedError(*result);
        } else {
            stat = ((RooRealVar*)args.find(("pdf_params_" + var).c_str()))->getError();
        }
        std::cout << "Stat uncertainty: " << stat << std::endl;
        std::cout << "Sys uncertainty: " << sys << std::endl;
        if (log10(stat) - log10(sys) < 2) {
            width_map[var] = std::make_pair(sys, stat);
        }
        width_map_all[var] = std::make_pair(sys, stat);
        
    } // End loop over fit parameters

    // Print systematic uncertainty results to file
    std::ofstream file("Results/" + set_name + ".param");
    for (auto width : width_map) {
        file << width.first << " " << width.second.first << " ("
            << width.second.second << ")" << std::endl;
    }
    file.close();
    std::ofstream file_all("Results/all/" + set_name + ".param");
    for (auto width : width_map_all) {
        file_all << width.first << " " << width.second.first << " ("
            << width.second.second << ")" << std::endl;
    }
    file_all.close();

    return 0;
}
