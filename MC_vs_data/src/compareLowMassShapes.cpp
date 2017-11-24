#include <string>
#include <map>

#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TIterator.h"

#include "RooFormulaVar.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooArgList.h"
#include "RooFitResult.h"

#include "RooHORNSdini.h"
#include "RooHILLdini.h"
#include "RooLITTLEHORNSdini.h"
#include "PlotStyle.hpp"


// ========================================================================
// Script to plot low mass shapes from Monte Carlo vs. shapes from data fit
// ========================================================================
int main(int argc, char * argv[]) {

    // Load plotting options
    setPlotStyle();
    gROOT->ForceStyle();

    // Get Monte Carlo fit histograms
    TFile * MC_file = TFile::Open("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/"
            "Histograms/lowMass.root", "READ");
    std::map<std::string, TH1F*> MC_fits;
    MC_fits["gamma_010"] = (TH1F*)MC_file->Get("fit_gamma_010");
    MC_fits["gamma_101"] = (TH1F*)MC_file->Get("fit_gamma_101");
    MC_fits["pi_010"] = (TH1F*)MC_file->Get("fit_pi_010");
    MC_fits["pi_101"] = (TH1F*)MC_file->Get("fit_pi_101");
    MC_fits["Bs_gamma_010"] = (TH1F*)MC_file->Get("fit_Bs_gamma_010");
    MC_fits["Bs_gamma_101"] = (TH1F*)MC_file->Get("fit_Bs_gamma_101");
    MC_fits["Bs_pi_010"] = (TH1F*)MC_file->Get("fit_Bs_pi_010");
    MC_fits["Bs_pi_101"] = (TH1F*)MC_file->Get("fit_Bs_pi_101");

    // Get data fit parameters
    std::map<std::string, RooRealVar*> data_params;
    std::map<std::string, RooAbsPdf*> data_fits;
    TFile * data_file = TFile::Open("/home/pullen/analysis/B02DKstar/Fit_data/"
            "Results/twoBody_2011:2012:2015:2016_split_binned.root", "READ");
    RooFitResult * data_result = (RooFitResult*)data_file->Get("fit_result");
    RooArgList floating_params = data_result->floatParsFinal();
    TIterator * it_float = floating_params.createIterator();
    RooRealVar * var;
    while ((var = (RooRealVar*)it_float->Next())) {
        data_params[var->GetName()] = new RooRealVar(var->GetName(), "", 
                var->getVal());
    }
    RooArgList fixed_params = data_result->constPars();
    TIterator * it_fixed = fixed_params.createIterator();
    while ((var = (RooRealVar*)it_fixed->Next())) {
        data_params[var->GetName()] = new RooRealVar(var->GetName(), "", 
                var->getVal());
    }

    // Check which parameters are in the map
    for (auto param : data_params) {
        std::cout << "Name: ";
        std::cout << param.first;
        std::cout << "; Value: ";
        std::cout << param.second->getVal() << std::endl;
    }


    // Extra parameters needed
    RooRealVar * shift = new RooRealVar("shift_zero", "", 0);
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 
            MC_fits["gamma_010"]->GetBinCenter(1), 
            MC_fits["gamma_010"]->GetBinCenter(MC_fits["gamma_010"]->GetNbinsX()));

    // Account for width smearing in data
    RooFormulaVar * sigma_gamma_010 = new RooFormulaVar("sigma_gamma_010", 
            "@0 * @1", RooArgList(*data_params["low_sigma_gamma_010_fixed"], 
                *data_params["sigma_smear"]));
    RooFormulaVar * sigma_gamma_101 = new RooFormulaVar("sigma_gamma_101", 
            "@0 * @1", RooArgList(*data_params["low_sigma_gamma_101_fixed"], 
                *data_params["sigma_smear"]));
    RooFormulaVar * sigma_pi_010 = new RooFormulaVar("sigma_pi_010", 
            "@0 * @1", RooArgList(*data_params["low_sigma_pi_010_fixed"], 
                *data_params["sigma_smear"]));
    RooFormulaVar * sigma_pi_101 = new RooFormulaVar("sigma_pi_101", 
            "@0 * @1", RooArgList(*data_params["low_sigma_pi_101_fixed"], 
                *data_params["sigma_smear"]));
    RooFormulaVar * Bs_sigma_gamma_010 = new RooFormulaVar("Bs_sigma_gamma_010", 
            "@0 * @1", RooArgList(*data_params["Bs_low_sigma_gamma_010_fixed"], 
                *data_params["sigma_smear_Bs"]));
    RooFormulaVar * Bs_sigma_gamma_101 = new RooFormulaVar("Bs_sigma_gamma_101", 
            "@0 * @1", RooArgList(*data_params["Bs_low_sigma_gamma_101_fixed"], 
                *data_params["sigma_smear_Bs"]));
    RooFormulaVar * Bs_sigma_pi_010 = new RooFormulaVar("Bs_sigma_pi_010", 
            "@0 * @1", RooArgList(*data_params["Bs_low_sigma_pi_010_fixed"], 
                *data_params["sigma_smear_Bs"]));
    RooFormulaVar * Bs_sigma_pi_101 = new RooFormulaVar("Bs_sigma_pi_101", 
            "@0 * @1", RooArgList(*data_params["Bs_low_sigma_pi_101_fixed"], 
                *data_params["sigma_smear_Bs"]));


    // Kinematic endpoints for Bs
    RooRealVar * delta_M = new RooRealVar("delta_M_fixed", "", 87.26);
    RooFormulaVar * Bs_a_gamma = new RooFormulaVar("Bs_a_gamma", "@0 + @1", 
            RooArgList(*delta_M, *data_params["low_a_gamma"]));
    RooFormulaVar * Bs_b_gamma = new RooFormulaVar("Bs_b_gamma", "@0 + @1", 
            RooArgList(*delta_M, *data_params["low_b_gamma"]));
    RooFormulaVar * Bs_a_pi = new RooFormulaVar("Bs_a_pi", "@0 + @1", 
            RooArgList(*delta_M, *data_params["low_a_pi"]));
    RooFormulaVar * Bs_b_pi = new RooFormulaVar("Bs_b_pi", "@0 + @1", 
            RooArgList(*delta_M, *data_params["low_b_pi"]));


    // Make PDFs
    std::cout << "Making gamma 010" << std::endl;
    data_fits["gamma_010"] = new RooHILLdini("gamma_010_data", "", *Bd_M,
            *data_params["low_a_gamma"], *data_params["low_b_gamma"], 
            *data_params["low_csi_gamma_010"], *shift, 
            *sigma_gamma_010, *data_params["low_ratio"],
            *data_params["low_frac"]);
    std::cout << "Making gamma 101" << std::endl;
    data_fits["gamma_101"] = new RooLITTLEHORNSdini("gamma_101_data", "", *Bd_M,
            *data_params["low_a_gamma"], *data_params["low_b_gamma"], 
            *data_params["low_csi_gamma_101"], *shift, 
            *sigma_gamma_101, *data_params["low_ratio"],
            *data_params["low_frac"], *shift);
    std::cout << "Making pi 010" << std::endl;
    data_fits["pi_010"] = new RooHORNSdini("pi_010_data", "", *Bd_M,
            *data_params["low_a_pi"], *data_params["low_b_pi"], 
            *data_params["low_csi_pi_010"], *shift, 
            *sigma_pi_010, *data_params["low_ratio"],
            *data_params["low_frac"]);
    std::cout << "Making pi 101" << std::endl;
    data_fits["pi_101"] = new RooHILLdini("pi_101_data", "", *Bd_M,
            *data_params["low_a_pi"], *data_params["low_b_pi"], 
            *data_params["low_csi_pi_101"], *shift, 
            *sigma_pi_101, *data_params["low_ratio"],
            *data_params["low_frac"]);
    std::cout << "Making Bs gamma 010" << std::endl;
    data_fits["Bs_gamma_010"] = new RooHILLdini("Bs_gamma_010_data", "", *Bd_M,
            *Bs_a_gamma, *Bs_b_gamma,
            *data_params["Bs_low_csi_gamma_010"], *shift, 
            *Bs_sigma_gamma_010, *data_params["low_ratio"],
            *data_params["low_frac"]);
    std::cout << "Making Bs gamma 101" << std::endl;
    data_fits["Bs_gamma_101"] = new RooLITTLEHORNSdini("Bs_gamma_101_data", "", *Bd_M,
            *Bs_a_gamma, *Bs_b_gamma,
            *data_params["Bs_low_csi_gamma_101"], *shift, 
            *Bs_sigma_gamma_101, *data_params["low_ratio"],
            *data_params["low_frac"], *shift);
    std::cout << "Making Bs pi 010" << std::endl;
    data_fits["Bs_pi_010"] = new RooHORNSdini("Bs_pi_010_data", "", *Bd_M,
            *Bs_a_pi, *Bs_b_pi,
            *data_params["Bs_low_csi_pi_010"], *shift, 
            *Bs_sigma_pi_010, *data_params["low_ratio"],
            *data_params["low_frac"]);
    std::cout << "Making Bs pi 101" << std::endl;
    data_fits["Bs_pi_101"] = new RooHILLdini("Bs_pi_101_data", "", *Bd_M,
            *Bs_a_pi, *Bs_b_pi,
            *data_params["Bs_low_csi_pi_101"], *shift, 
            *Bs_sigma_pi_101, *data_params["low_ratio"],
            *data_params["low_frac"]);


    // Plot
    TCanvas * canvas = new TCanvas("canvas", "", 500, 400);
    for (auto fit : MC_fits) {

        // Plot MC
        fit.second->SetStats(false);
        fit.second->SetLineColor(kBlue);
        fit.second->Scale(1/fit.second->Integral());
        fit.second->Draw("C");

        // Make data histogram
        TH1F * data_hist = (TH1F*)data_fits[fit.first]->createHistogram(
                ("h_data_" + fit.first).c_str(), *Bd_M, 
                RooFit::Binning(fit.second->GetNbinsX()));

        // Plot data
        data_hist->SetStats(false);
        data_hist->SetLineColor(kRed);
        data_hist->Scale(1/data_hist->Integral());
        data_hist->Draw("C SAME");

        // Save and clear canvas
        canvas->SaveAs(("Plots/fit_" + fit.first + ".pdf").c_str());
        canvas->Clear();
    }

    return 0;
}
