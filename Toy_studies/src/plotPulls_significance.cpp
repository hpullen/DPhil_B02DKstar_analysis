#include <iostream>

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
    std::string set_name = "tight_pid_twoBody_only";

    // Iterator for floating vars
    TFile * file = TFile::Open(("/data/lhcb/users/pullen/B02DKstar/toys/significance/"
            + set_name + "_1_floating.root").c_str());
    RooFitResult * result = (RooFitResult*)file->Get("fitresult_simPdf_hmaster_binned");
    RooArgList vars = result->floatParsFinal();
    TIterator * it = vars.createIterator();
    RooRealVar * var = 0;

    // Read in toy files
    TChain * float_tree = new TChain("float_tree");
    float_tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/significance/" + set_name + "_*_floating.root").c_str());
    TChain * fixed_tree = new TChain("fixed_tree");
    fixed_tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/significance/" + set_name + "_*_floating.root").c_str());

    // Loop through variables and plot pulls for each
    while ((var = (RooRealVar*)it->Next())) {

        // Get name
        std::string varname = var->GetName();

        // Make histogram to hold pulls
        TH1F * pull_hist = new TH1F(("pull_hist_" + varname).c_str(), "", 30,
                -10, 10);

        // Create histograms from toy tree
        // Set up variables
        double float_value = 0;
        double fixed_value = 0;
        double error = 0;
        double status = 0;
        double covQual = 0;
        double EDM = 0;
        float_tree->SetBranchAddress(("float_value_" + varname).c_str(), &float_value);
        fixed_tree->SetBranchAddress(("fixed_value_" + varname).c_str(), &fixed_value);
        float_tree->SetBranchAddress("status", &status);
        float_tree->SetBranchAddress("covQual", &covQual);
        float_tree->SetBranchAddress("EDM", &EDM);

        // Loop through toys 
        for (int i = 0; i < float_tree->GetEntries(); i++) {

            // Get entry and skip if bad
            float_tree->GetEntry(i);
            fixed_tree->GetEntry(i);
            if (status != 0 || covQual != 3 || EDM > 0.001) continue;

            // Fill histograms and calculate pull
            pull_hist->Fill((fixed_value - float_value)/error);
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
            canvas->cd(3);
            gauss_fit->Draw("C SAME");
            pull_hist->Draw("E SAME");
        }
        else {
            std::cout << "Could not fit pull for variable " << varname << 
                std::endl;
        }

        // Save the canvas
        canvas->SaveAs(("Plots/significance/pulls_" + varname + ".pdf").c_str());

    } // End loop over fit parameters

            return 0;
}


            










   
