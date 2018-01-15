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
        std::cout << "Usage: ./PlotPulls_significance <name>" << std::endl;
        return -1;
    }
    std::string set_name = argv[1];

    // Iterator for floating vars
    TFile * file = TFile::Open(("/data/lhcb/users/pullen/B02DKstar/toys/significance/"
            + set_name + "_1.root").c_str());
    RooFitResult * result = (RooFitResult*)file->Get("fitresult_simPdf_hmaster_binned");
    RooArgList vars = result->floatParsFinal();
    TIterator * it = vars.createIterator();
    RooRealVar * var = 0;

    // Read in toy files
    TChain * toy_tree = new TChain("toy_tree");
    toy_tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/significance/" + set_name + "_*.root").c_str());
    std::cout << "Loaded toy tree with " << toy_tree->GetEntries() << " entries." 
        << std::endl;

    // Map to hold means of Gaussians
    std::map<std::string, double> mean_map;

    // Set up status
    int status = 0;
    toy_tree->SetBranchAddress("status", &status);

    // Loop through variables and plot pulls for each
    while ((var = (RooRealVar*)it->Next())) {

        // Get name
        std::string varname = var->GetName();

        // Make histogram to hold pulls
        double min = -10;
        double max = 10;
        TH1F * pull_hist = new TH1F(("pull_hist_" + varname).c_str(), "", 10,
                min, max);
        TH1F * init_hist = new TH1F(("init_hist_" + varname).c_str(), "", 10, min, 
                max);
        
        // Create histograms from toy tree
        // Set up variables
        double init_value = 0;
        double signal_value = 0;
        double signal_error = 0;
        double nosignal_value = 0;
        double nosignal_error = 0;
        toy_tree->SetBranchAddress(("init_value_" + varname).c_str(), &init_value);
        toy_tree->SetBranchAddress(("signal_value_" + varname).c_str(), &signal_value);
        toy_tree->SetBranchAddress(("signal_error_" + varname).c_str(), &signal_error);
        toy_tree->SetBranchAddress(("nosignal_value_" + varname).c_str(), &nosignal_value);
        toy_tree->SetBranchAddress(("nosignal_error_" + varname).c_str(), &nosignal_error);

        // Loop through toys
        std::cout << "Calculating pulls for variable: " << varname << std::endl;
        for (int i = 0; i < toy_tree->GetEntries(); i++) {
            // Fill histograms and calculate pull
            toy_tree->GetEntry(i);
            if (status != 0) continue;
            double pull = (nosignal_value - signal_value)/
                    sqrt(signal_error * signal_error + nosignal_error * nosignal_error);
            pull_hist->Fill(pull);
            double pull_init = (signal_value - init_value)/signal_error;
            init_hist->Fill(pull_init);
        }

        // Make canvas
        TCanvas * canvas = new TCanvas(("canvas_" + varname).c_str(), "", 500, 400);

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
        pull_hist->SetLineWidth(1);
        pull_hist->Draw("E");

        // Fit the pull histogram with a Gaussian
        if (pull_hist->Integral() != 0) {

            // Perform fit
            pull_hist->Fit("gaus");
            TF1 * gauss_fit = pull_hist->GetFunction("gaus");
            gauss_fit->SetLineColor(kBlue);
            gauss_fit->SetLineWidth(2);

            // Draw
            gauss_fit->Draw("C SAME");
            pull_hist->Draw("E SAME");

            // Add to map
            mean_map[varname] = gauss_fit->GetParameter("Mean");
        } else {
            std::cout << "Could not fit pull for variable " << varname <<
                std::endl;
        }
        // Save the canvas
        canvas->SaveAs(("Plots/significance/pulls_" + set_name + "_" + varname + 
                    ".pdf").c_str());
        delete canvas;
    
        // Set up initial vs. final plot
        TCanvas * init_canv = new TCanvas("init", "", 500, 400);
        init_hist->SetLineWidth(1);
        init_hist->Draw("E");

        // Fit the initial vs. final value pull histogram with a Gaussian
        if (init_hist->Integral() != 0) {

            // Perform fit
            init_hist->Fit("gaus");
            TF1 * gauss_fit = init_hist->GetFunction("gaus");
            gauss_fit->SetLineColor(kBlue);
            gauss_fit->SetLineWidth(2);

            // Draw
            gauss_fit->Draw("C SAME");
            init_hist->Draw("E SAME");

        }
        else {
            std::cout << "Could not fit pull for variable " << varname <<
                std::endl;
        }
        init_canv->SaveAs(("Plots/significance/init_vs_final_pulls_" + set_name + 
                    "_" + varname + ".pdf").c_str());
        delete init_canv;

        
    } // End loop over fit parameters

    // Plot significance for each toy
    double significance = 0;
    toy_tree->SetBranchAddress("significance", &significance);
    TH1F * sig_hist = new TH1F("significance", "", 20, 0, 10);
    for (int i = 0; i < toy_tree->GetEntries(); i++) {
        toy_tree->GetEntry(i);
        if (status !=0) continue;
        sig_hist->Fill(significance);
    }    
    TCanvas * canvas = new TCanvas("canvas", "", 500, 400);

    // Perform fit
    sig_hist->Fit("gaus");
    TF1 * gauss_fit = sig_hist->GetFunction("gaus");
    gauss_fit->SetLineColor(kBlue);
    gauss_fit->SetLineWidth(2);

    // Draw
    gauss_fit->Draw("C SAME");
    sig_hist->SetLineWidth(1);
    sig_hist->SetMarkerStyle(2);
    sig_hist->Draw("P SAME");
    canvas->SaveAs(("Plots/significance/significance_" + set_name + ".pdf").c_str());

    // Print pull mean results
    std::cout << std::endl;
    for (auto mean : mean_map) {
        std::cout << mean.first << " " << mean.second << std::endl;
    }

    return 0;
}
