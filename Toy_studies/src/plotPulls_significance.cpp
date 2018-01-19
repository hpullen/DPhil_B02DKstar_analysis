#include <iostream>
#include <map>

#include "TCanvas.h"
#include "TChain.h"
#include "TCut.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TIterator.h"
#include "TLegend.h"
#include "TPad.h"
#include "TStyle.h"

#include "RooArgList.h"
#include "RooFitResult.h"
#include "RooGaussian.h"
#include "RooRealVar.h"

#include "PlotStyle.hpp"


// =================================================
// Function to find min and max of histogram entries
// =================================================
std::pair<double, double> getMinAndMax(TTree * tree, std::string name1, std::string name2) {

    // Set up variables
    double var1 = 0;
    double var2 = 0;
    int fit_status = 0;
    tree->SetBranchAddress(name1.c_str(), &var1);
    tree->SetBranchAddress(name2.c_str(), &var2);
    tree->SetBranchAddress("status", &fit_status);

    // Variables for min and max
    double min = 0;
    double max = 0;
    bool filled = false;

    // Loop through entries
    for(int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        if (fit_status != 0) continue;
        if (!filled) {
            min = (var1 < var2) ? var1 : var2;
            max = (var1 > var2) ? var1 : var2;
            filled = true;
        } else {
            if (var1 < min) min = var1;
            if (var2 < min) min = var2;
            if (var1 > max) max = var1;
            if (var2 > max) max = var2;
        }
    }
    return std::make_pair(min, max);
}


// ============================================
// Function to get latex-style name of variable
// ============================================
std::string getLatexName(std::string name) {
    if (name == "Bs_low_frac_010") return "#alpha^{s}_{010} (B_{s} helicity fraction)";
    else if (name == "Bs_sigma_L") return "#sigma_{s} (B_{s} signal width)";
    else if (name == "R_KK_vs_Kpi") return "R_{KK}";
    else if (name == "R_KK_vs_Kpi_low") return "R^{low}_{KK}";
    else if (name == "R_KK_vs_piK_Bs") return "R^{s}_{KK}";
    else if (name == "R_piK_vs_Kpi") return "R_{ADS}";
    else if (name == "R_piK_vs_Kpi_low") return "R^{low}_{ADS}";
    else if (name == "R_pipi_vs_Kpi") return "R_{#pi#pi}";
    else if (name == "R_pipi_vs_Kpi_low") return "R^{low}_{#pi#pi}";
    else if (name == "R_pipi_vs_piK_Bs") return "R^{s}_{#pi#pi}";
    else if (name == "low_frac_010_GLW") return "#alpha^{GLW}_{010} (KK/#pi#pi helicity fraction)";
    else if (name == "low_frac_010_Kpi") return "#alpha^{K#pi}_{010} (K#pi helicity fraction)";
    else if (name == "low_frac_010_piK") return "#alpha^{#pi K}_{010} (#pi K helicity fraction)";
    else if (name == "n_Bs_low_piK") return "N^{#pi K}_{B_{s} low}";
    else if (name == "n_Bs_piK") return "N^{#pi K}_{B_{s}}";
    else if (name == "n_expo_KK") return "N^{KK}_{comb}";
    else if (name == "n_expo_Kpi") return "N^{K#pi}_{comb}";
    else if (name == "n_expo_piK") return "N^{#pi K}_{comb}";
    else if (name == "n_expo_pipi") return "N^{#pi#pi}_{comb}";
    else if (name == "n_low_Kpi") return "N^{K#pi}_{low}";
    else if (name == "n_rho_Kpi") return "N^{K#pi}_{#rho^{0}}";
    else if (name == "n_signal_Kpi") return "N^{K#pi}_{sig}";
    else if (name == "shift") return "#delta (global shift)";
    else if (name == "signal_sigma_L") return "#sigma (signal width)";
    else if (name == "slope_KK") return "c_{KK} (KK combinatorial slope)";
    else if (name == "slope_Kpi") return "c_{K#pi} (K#pi combinatorial slope)";
    else if (name == "slope_pipi") return "c_{#pi#pi} (#pi#pi combinatorial slope)";
    else {
        std::cout << "Couldn't find title string for variable: " << name << std::endl;
        return "";
    }
}


// ===========================================================================
// Main function: plot values/errors/pulls for each variable, and significance
// ===========================================================================
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
    double init_value = 0;
    double signal_value = 0;
    double signal_error = 0;
    double nosignal_value = 0;
    double nosignal_error = 0;
    int status = 0;
    toy_tree->SetBranchAddress("status", &status);

    // Loop through variables and plot pulls for each
    while ((var = (RooRealVar*)it->Next())) {

        // Get name
        std::string varname = var->GetName();

        // Make histogram to hold pulls
        TH1F * pull_hist;
        if (varname == "Bs_low_frac_010" || varname == "R_piK_vs_Kpi_low" ||
                varname == "low_frac_010_piK" || varname == "n_Bs_low_piK" ||
                varname == "n_expo_piK") {
            pull_hist = new TH1F(("pull_hist_" + varname).c_str(), "", 200, -1.5, 1.5);
        } else if (varname == "R_piK_vs_Kpi") {
            pull_hist = new TH1F(("pull_hist_" + varname).c_str(), "", 200, -10, 5);
        } else {
            pull_hist = new TH1F(("pull_hist_" + varname).c_str(), "", 200, -0.5, 0.5);
        }
        TH1F * init_hist = new TH1F(("init_hist_" + varname).c_str(), "", 30, -5, 5);

        // Make histograms to hold fit values
        std::pair<double, double> min_max_value = getMinAndMax(toy_tree, "signal_value_" + varname, "nosignal_value_" + varname);
        double min_value = min_max_value.first;
        double max_value = min_max_value.second;
        TH1F * signal_value_hist = new TH1F(("signal_value_hist_" + varname).c_str(), "", 30,
                min_value - (max_value - min_value)/8,
                max_value + (max_value - min_value)/8);
        TH1F * nosignal_value_hist = new TH1F(("nosignal_value_hist_" + varname).c_str(), "", 30,
                min_value - (max_value - min_value)/8,
                max_value + (max_value - min_value)/8);

        // Make histograms to hold fit errors
        std::pair<double, double> min_max_error = getMinAndMax(toy_tree, "signal_error_" + varname, "nosignal_error_" + varname);
        double min_error = min_max_error.first;
        double max_error = min_max_error.second;
        TH1F * signal_error_hist = new TH1F(("signal_error_hist_" +
                    varname).c_str(), "", 30,
                min_error - (max_error - min_error)/8,
                max_error + (max_error - min_error)/8);
        TH1F * nosignal_error_hist = new TH1F(("nosignal_error_hist_" +
                    varname).c_str(), "", 30,
                min_error - (max_error - min_error)/8,
                max_error + (max_error - min_error)/8);

        // Create histograms from toy tree
        // Set up variables
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
            signal_value_hist->Fill(signal_value);
            nosignal_value_hist->Fill(nosignal_value);
            signal_error_hist->Fill(signal_error);
            nosignal_error_hist->Fill(nosignal_error);
        }

        // // Make canvas
        // TCanvas * canvas = new TCanvas(("canvas_" + varname).c_str(), "", 1500, 400);
        // canvas->Divide(3, 1, 0.0001, 0.0001);

        // // Set up plotting
        // gStyle->SetFrameBorderSize(1);
        // gStyle->SetCanvasBorderSize(1);
        // gStyle->SetOptStat(0);
        // gStyle->SetOptFit(0001);
        // gStyle->SetStatW(0.15);
        // gStyle->SetStatH(0.2);
        // gStyle->SetStatX(0.85);
        // gStyle->SetStatColor(1);
        // gStyle->SetStatBorderSize(1);

        // // Plot values and errors
        // signal_value_hist->SetLineColor(kRed);
        // signal_value_hist->SetLineWidth(1);
        // signal_value_hist->GetYaxis()->SetRangeUser(0, signal_value_hist->GetMaximum() * 1.2);
        // signal_value_hist->GetXaxis()->SetTitle(getLatexName(varname).c_str());
        // nosignal_value_hist->SetLineColor(ANABlue);
        // nosignal_value_hist->SetLineWidth(1);
        // signal_error_hist->SetLineColor(kRed);
        // signal_error_hist->SetLineWidth(1);
        // signal_error_hist->GetYaxis()->SetRangeUser(0, signal_error_hist->GetMaximum() * 1.2);
        // signal_error_hist->GetXaxis()->SetTitle("Error");
        // nosignal_error_hist->SetLineColor(ANABlue);
        // nosignal_error_hist->SetLineWidth(1);
        // canvas->cd(1);
        // signal_value_hist->Draw();
        // nosignal_value_hist->Draw("SAME");

        // // Make legend
        // TLegend * leg = new TLegend(0.55, 0.7, 0.85, 0.9);
        // leg->SetFillStyle(0);
        // leg->AddEntry(signal_value_hist, "Normal fit");
        // leg->AddEntry(nosignal_value_hist, "Null hypothesis");
        // leg->Draw();

        // // Plot errors
        // canvas->cd(2);
        // signal_error_hist->Draw();
        // nosignal_error_hist->Draw("SAME");
        // leg->Draw();

        // // Plot histograms
        // canvas->cd(3);
        // pull_hist->SetLineWidth(1);
        // pull_hist->GetXaxis()->SetTitle("Pull");
        // pull_hist->GetYaxis()->SetRangeUser(0, pull_hist->GetMaximum() * 1.6);
        // pull_hist->Draw("E");

        // // Fit the pull histogram with a Gaussian
        // if (pull_hist->Integral() != 0) {

            // // Perform fit
            // pull_hist->Fit("gaus");
            // TF1 * gauss_fit = pull_hist->GetFunction("gaus");
            // gauss_fit->SetLineColor(ANABlue);
            // gauss_fit->SetLineWidth(1);

            // // Draw
            // gauss_fit->Draw("C SAME");
            // pull_hist->Draw("E SAME");

            // // Add to map
            // mean_map[varname] = gauss_fit->GetParameter("Mean");
        // } else {
            // std::cout << "Could not fit pull for variable " << varname <<
                // std::endl;
        // }
        // // Save the canvas
        // canvas->SaveAs(("Plots/significance/pulls_" + set_name + "_" + varname +
                    // ".pdf").c_str());
        // delete canvas;

        // // Set up initial vs. final plot
        // TCanvas * init_canv = new TCanvas("init", "", 500, 400);
        // init_hist->SetLineWidth(1);
        // init_hist->Draw("E");

        // // Fit the initial vs. final value pull histogram with a Gaussian
        // if (init_hist->Integral() != 0) {

            // // Perform fit
            // init_hist->Fit("gaus");
            // TF1 * gauss_fit = init_hist->GetFunction("gaus");
            // gauss_fit->SetLineColor(kBlue);
            // gauss_fit->SetLineWidth(2);

            // // Draw
            // gauss_fit->Draw("C SAME");
            // init_hist->Draw("E SAME");

        // }
        // else {
            // std::cout << "Could not fit pull for variable " << varname <<
                // std::endl;
        // }
        // init_canv->SaveAs(("Plots/significance/init_vs_final_pulls_" + set_name +
                    // "_" + varname + ".pdf").c_str());
        // delete init_canv;

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
