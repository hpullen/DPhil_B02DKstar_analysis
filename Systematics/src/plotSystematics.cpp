#include <iostream>
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
        "A_Kpi",
        "A_KK_blind",
        "A_pipi_blind",
        "A_piK_Bs",
        "A_KK_Bs",
        "A_pipi_Bs",
        "R_plus_blind",
        "R_minus_blind",
        "R_KK_vs_Kpi_blind",
        "R_pipi_vs_Kpi_blind",
        "R_KK_vs_piK_Bs",
        "R_pipi_vs_piK_Bs",
        "R_KK_vs_piK_Bs_low",
        "R_pipi_vs_piK_Bs_low"
    };

    // Read in toy files
    TChain * sys_tree = new TChain("sys_tree");
    sys_tree->Add(("/data/lhcb/users/pullen/B02DKstar/systematics/" + set_name + "_*.root").c_str());
    std::cout << "Loaded toy tree with " << sys_tree->GetEntries() << " entries." 
        << std::endl;

    // Map to hold means of Gaussians
    std::map<std::string, double> width_map;

    // Set up status
    int status = 0;
    sys_tree->SetBranchAddress("status", &status);

    // Loop through variables and plot spread of each
    for (auto var : obs) {

        // Make histogram to hold pulls
        double centre;
        double spread;
        if (var == "A_KK_Bs") {
            centre = -0.07;
            spread = 0.01;
        } else if (var == "A_KK_blind") {
            centre = 0.02;
            spread = 0.01;
        } else if (var == "A_Kpi") {
            centre = 0.028;
            spread = 0.01;
        } else if (var == "A_piK_Bs") {
            centre = -0.01;
            spread = 0.01;
        } else if (var == "A_pipi_Bs") {
            centre = 0.024;
            spread = 0.01;
        } else if (var == "A_pipi_blind") {
            centre = -0.1;
            spread = 0.1;
        } else if (var == "R_KK_vs_Kpi_blind") {
            centre = 0.1;
            spread = 0.01;
        } else if (var == "R_KK_vs_piK_Bs") {
            centre = 0.1;
            spread = 0.006;
        } else if (var == "R_minus_blind") {
            centre = 0.0195;
            spread = 0.009;
        } else if (var == "R_plus_blind") {
            centre = 0.0195;
            spread = 0.009;
        } else if (var == "R_pipi_vs_Kpi_blind") {
            centre = 0.08;
            spread = 0.009;
        } else if (var == "R_pipi_vs_piK_Bs") {
            centre = 0.04;
            spread = 0.003;
        }
        double min = centre - spread;
        double max = centre + spread;
        TH1F * sys_hist = new TH1F(("sys_hist_" + var).c_str(), "", 100,
                min, max);
        
        // Create histograms from toy tree
        // Set up variables
        double value = 0;
        sys_tree->SetBranchAddress(var.c_str(), &value);

        // Loop through toys
        std::cout << "Calculating systematic for variable: " << var << std::endl;
        for (int i = 0; i < sys_tree->GetEntries(); i++) {
            // Fill histograms and calculate pull
            sys_tree->GetEntry(i);
            if (status != 0) continue;
            sys_hist->Fill(value);
        }

        // Make canvas
        TCanvas * canvas = new TCanvas(("canvas_" + var).c_str(), "", 500, 400);

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

            // Add to map
            // width_map[varname] = gauss_fit->GetParameter("Mean");
        } else {
            std::cout << "Could not fit variable " << var <<
                std::endl;
        }
        // Save the canvas
        canvas->SaveAs(("Plots/systematic_" + set_name + "_" + var + 
                    ".pdf").c_str());
        delete canvas;
        
    } // End loop over fit parameters

    // Print systematic uncertainty results
    std::cout << std::endl;
    for (auto width : width_map) {
        std::cout << width.first << " " << width.second << std::endl;
    }

    return 0;
}
