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

    // Get latest fit result and list of floating vars
    TFile * file = TFile::Open("/home/pullen/analysis/B02DKstar/Fit_data/Results/"
            "twoBody_2011:2012:2015:2016_split_binned.root", "READ");
    RooFitResult * result = (RooFitResult*)file->Get("fit_result");
    RooArgList vars = result->floatParsFinal();

    // Iterator for floating vars
    TIterator * it = vars.createIterator();
    RooRealVar * var = 0;

    // Read in toy files
    TChain * toy_tree = new TChain("toy_tree");
    toy_tree->Add("/data/lhcb/users/pullen/B02DKstar/toys/twoBody/19_11_17/*.root");

    // Loop through variables and plot pulls for each
    while ((var = (RooRealVar*)it->Next())) {

        // Get name
        std::string varname = var->GetName();

        // Only do for interesting physics parameters
        bool is_interesting = (varname == "A_Kpi" || varname == "A_KK_blind" ||
                varname == "A_pipi_blind" || varname == "A_piK_Bs" || 
                varname == "A_KK_Bs" || varname == "A_pipi_Bs" ||
                varname == "R_plus_blind" || varname == "R_minus_blind" ||
                varname == "R_KK_vs_Kpi_blind" || 
                varname == "R_pipi_vs_Kpi_blind" || 
                varname == "R_KK_vs_piK_Bs" || 
                varname == "R_pipi_vs_piK_Bs");
        if (!is_interesting) continue;

        // Latex-style variable name for axis
        std::string latex_name;
        if (varname == "A_Kpi") latex_name = "A_{K#pi}";
        else if (varname == "A_KK_blind") latex_name = "A_{KK} (blind)";
        else if (varname == "A_pipi_blind") latex_name = "A_{#pi#pi} (blind)";
        else if (varname == "A_piK_Bs") latex_name = "A_{#pi K, s}";
        else if (varname == "A_KK_Bs") latex_name = "A_{KK, s}";
        else if (varname == "A_pipi_Bs") latex_name = "A_{#pi#pi, s}";
        else if (varname == "R_plus_blind") latex_name = "R_{+} (blind)";
        else if (varname == "R_minus_blind") latex_name = "R_{-} (blind)";
        else if (varname == "R_KK_vs_Kpi blind") latex_name = "R_{KK} (blind)";
        else if (varname == "R_pipi_vs_Kpi blind") latex_name = "R_{#pi#pi} (blind)";
        else if (varname == "R_KK_vs_piK_Bs blind") latex_name = "R_{KK, s}";
        else if (varname == "R_pipi_vs_piK_Bs blind") latex_name = "R_{#pi#pi, s}";

        // Make histogram to hold pulls, errors, and values
        double min;
        double max;
        if (varname.find("A_") != std::string::npos) {
            min = -0.2;
            max = 0.2;
        } else {
            min = 0;
            max = 1.5;
        }
        TH1F * value_hist = new TH1F(("value_hist" + varname).c_str(), "", 30,
                min, max);
        TH1F * error_hist = new TH1F(("error_hist" + varname).c_str(), "", 30,
                0, 0.1);
        TH1F * pull_hist = new TH1F(("pull_hist_" + varname).c_str(), "", 30,
                -10, 10);

        // Create histograms from toy tree
        // Set up variables
        double init_value = 0;
        double final_value = 0;
        double error = 0;
        double status = 0;
        double covQual = 0;
        double EDM = 0;
        toy_tree->SetBranchAddress(("init_value_" + varname).c_str(), &init_value);
        toy_tree->SetBranchAddress(("final_value_" + varname).c_str(), &final_value);
        toy_tree->SetBranchAddress(("error_value_" + varname).c_str(), &error);
        toy_tree->SetBranchAddress("status", &status);
        toy_tree->SetBranchAddress("covQual", &covQual);
        toy_tree->SetBranchAddress("EDM", &EDM);

        // Loop through toys 
        for (int i = 0; i < toy_tree->GetEntries(); i++) {

            // Get entry and skip if bad
            toy_tree->GetEntry(i);
            if (status != 0 || covQual != 3 || EDM > 0.001) continue;

            // Fill histograms and calculate pull
            value_hist->Fill(final_value);
            error_hist->Fill(error);
            pull_hist->Fill((final_value - init_value)/error);
        }


        // Make canvas
        TCanvas * canvas = new TCanvas(("canvas_" + varname).c_str(), "", 1500, 400);
        canvas->Divide(3, 1, 0.0001, 0.0001);


        // Plot histograms
        gStyle->SetFrameBorderSize(1);
        gStyle->SetCanvasBorderSize(1);
        value_hist->GetXaxis()->SetTitle(latex_name.c_str());
        value_hist->SetLineWidth(1);
        canvas->cd(1);
        value_hist->Draw("HIST");
        error_hist->GetXaxis()->SetTitle((latex_name + " error").c_str());
        error_hist->SetLineWidth(1);
        canvas->cd(2);
        error_hist->Draw("HIST");
        gStyle->SetOptStat(0);
        gStyle->SetOptFit(0001);
        gStyle->SetStatW(0.15);
        gStyle->SetStatH(0.2);
        gStyle->SetStatX(0.85);
        gStyle->SetStatColor(1);
        gStyle->SetStatBorderSize(1);
        pull_hist->GetXaxis()->SetTitle((latex_name + " pulls").c_str());
        pull_hist->SetLineWidth(1);
        canvas->cd(3);
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
        canvas->SaveAs(("Plots/pulls_" + varname + ".pdf").c_str());

    } // End loop over fit parameters

            return 0;
}


            










   
