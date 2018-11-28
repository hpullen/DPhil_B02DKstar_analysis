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

#include "RooArgList.h"
#include "RooFitResult.h"
#include "RooGaussian.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"

#include "PlotStyle.hpp"

int main (int argc, char * argv[]) {

    // Set plotting style
    setPlotStyle();

    // Name of toy set
    if (argc != 2) {
        std::cout << "Usage: ./PlotSystematics <name>" << std::endl;
        return -1;
    }
    std::string set_name = argv[1];

    // List of observables
    std::vector<std::string> obs = {
        "A_signal_Kpi",
        "A_signal_Kpipipi",
        "A_signal_KK_run1_blind",
        "A_signal_KK_run2_blind",
        "A_signal_pipi_run1_blind",
        "A_signal_pipi_run2_blind",
        "A_signal_pipipipi_run2_blind",
        "A_Bs_piK",
        "A_Bs_piKpipi",
        "A_Bs_KK_run1",
        "A_Bs_KK_run2",
        "A_Bs_pipi_run1",
        "A_Bs_pipi_run2",
        "A_Bs_pipipipi_run2",
        "R_ds_KK_run1_blind",
        "R_ds_KK_run2_blind",
        "R_ds_pipi_run1_blind",
        "R_ds_pipi_run2_blind",
        "R_ds_pipipipi_run2_blind",
        "R_signal_piK_plus_blind",
        "R_signal_piK_minus_blind",
        "R_signal_piKpipi_plus_blind",
        "R_signal_piKpipi_minus_blind",
        "R_signal_KK_run1_blind",
        "R_signal_KK_run2_blind",
        "R_signal_pipi_run2_blind",
        "R_signal_pipi_run1_blind",
        "R_signal_pipipipi_run2_blind"
    };

    // Only use R_ds if considering fs/fd (not used in fit)
    if (set_name == "fs_fd") {
        obs = {"R_ds_KK_run1_blind",
            "R_ds_KK_run2_blind",
            "R_ds_pipi_run1_blind",
            "R_ds_pipi_run2_blind",
            "R_ds_pipipipi_run2_blind"};
    }

    // Read in toy files
    TChain * sys_tree = new TChain("sys_tree");
    sys_tree->Add(("/data/lhcb/users/pullen/B02DKstar/systematics/" + set_name + "/*.root").c_str());
    std::cout << "Loaded systematics tree with " << sys_tree->GetEntries() << " entries." 
        << std::endl;

    // Map to hold means of Gaussians
    std::map<std::string, std::pair<double, double>> width_map;
    std::map<std::string, std::pair<double, double>> width_map_all;

    // Read in original fit result (for stat uncertainty)
    TFile * res_file = TFile::Open("../Fit_data/Results/twoAndFourBody_data_split.root", "READ");
    RooFitResult * result = (RooFitResult*)res_file->Get("fit_result");
    RooArgList args = result->floatParsFinal();

    // Loop through variables and plot spread of each
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    for (auto var : obs) {

        // Make histogram
        sys_tree->Draw((var + ">>hist_" + var).c_str(), "status == 0 && covQual == 3");
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
        if (sys_hist->Integral() != 0) {

            // Perform fit
            sys_hist->Fit("gaus");
            TF1 * gauss_fit = sys_hist->GetFunction("gaus");
            gauss_fit->SetLineColor(kBlue);
            gauss_fit->SetLineWidth(2);

            // Draw
            gauss_fit->Draw("C SAME");
            sys_hist->Draw("E SAME");

            // Add to map if not 2 order of magnitude smaller than stat
            double stat;
            if (var.find("R_ds_") != std::string::npos) {
                RooFormulaVar * fvar = (RooFormulaVar*)res_file->Get(var.c_str());
                stat = fvar->getPropagatedError(*result);
            } else {
                stat = ((RooRealVar*)args.find(("pdf_params_" + var).c_str()))->getError();
            }
            double sys = gauss_fit->GetParameter("Sigma");
            std::cout << "Stat uncertainty: " << stat << std::endl;
            std::cout << "Sys uncertainty: " << sys << std::endl;
            if (log10(stat) - log10(sys) < 2) {
                width_map[var] = std::make_pair(sys, stat);
            }
            width_map_all[var] = std::make_pair(sys, stat);

        } else {
            std::cout << "Could not fit variable " << var <<
                std::endl;
        }
        // Save the canvas
        canvas->SaveAs(("Plots/" + set_name + "/" + var + ".pdf").c_str());
        
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
