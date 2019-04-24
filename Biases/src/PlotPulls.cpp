// Standard includes
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

// ROOT includes
#include "TChain.h"
#include "TPaveText.h"
#include "TF1.h"
#include "TLine.h"
#include "TCut.h"
#include "TH1F.h"
#include "TEventList.h"
#include "TCanvas.h"
#include "TFile.h"

// RooFit includes
#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooFormulaVar.h"

// Local library includes
#include "PlotStyle.hpp"


// Script to plot results and pulls for fit parameters
// Get names of certain observables
std::string get_name(std::string par) {

    // Map of names 
    std::map<std::string, std::string> names = {
        {"A_signal_Kpi", "A^{K#pi}"},
        {"R_signal_piK_plus", "R_{#pi K}^{+}"},
        {"R_signal_piK_minus", "R_{#pi K}^{-}"},
        {"A_Bs_piK", "A_{s}^{#pi K}"},
        {"A_signal_Kpipipi", "A^{K#pi#pi#pi}"},
        {"R_signal_piKpipi_plus", "R_{#pi K#pi#pi}^{+}"},
        {"R_signal_piKpipi_minus", "R_{#pi K#pi#pi}^{-}"},
        {"A_Bs_piKpipi", "A_{s}^{#pi K#pi#pi}"},
        {"R_signal_pipipipi_run2", "R^{4#pi}"},
        {"A_signal_pipipipi_run2", "A^{4#pi}"},
        {"R_signal_pipipipi", "R^{4#pi}"},
        {"A_signal_pipipipi", "A^{4#pi}"},
        {"R_ds_pipipipi_run2", "R_{ds}^{4#pi}"},
        {"R_Bs_pipipipi_run2", "R_{s}^{4#pi}"},
        {"A_Bs_pipipipi_run2", "A_{s}^{4#pi}"},
        {"A_Bs_pipipipi", "A_{s}^{4#pi}"}
    };

    // Add GLW variables
    for (std::string run : {"", "1", "2"}) {
        std::string comma_run = (run == "") ? "" : "," + run;
        std::string name_run = (run == "") ? "" : "_run" + run;
        names["A_signal_Kpi" + name_run] = "A^{K#pi" + comma_run + "}";
        names["A_signal_Kpipipi" + name_run] = "A^{K#pi#pi#pi" + comma_run + "}";
        names["A_Bs_piK" + name_run] = "A_{s}^{#pi K" + comma_run + "}";
        names["A_Bs_piKpipi" + name_run] = "A_{s}^{#pi K#pi#pi" + comma_run + "}";
        names["R_signal_piK" + name_run + "_plus"] = "R_{#pi K}^{+" + comma_run + "}";
        names["R_signal_piK" + name_run + "_minus"] = "R_{#pi K}^{-" + comma_run + "}";
        names["R_signal_piKpipi" + name_run + "_plus"] = "R_{#pi K#pi#pi}^{+" + comma_run + "}";
        names["R_signal_piKpipi" + name_run + "_minus"] = "R_{#pi K#pi#pi}^{-" + comma_run + "}";
        names["A_signal_KK" + name_run] = "A^{KK" + comma_run + "}";
        names["R_signal_KK" + name_run] = "R^{KK" + comma_run + "}";
        names["R_ds_KK" + name_run] = "R_{ds}^{KK" + comma_run + "}";
        names["R_Bs_KK" + name_run] = "R_{s}^{KK" + comma_run + "}";
        names["A_Bs_KK" + name_run] = "A_{s}^{KK" + comma_run + "}";
        names["A_signal_pipi" + name_run] = "A^{#pi#pi" + comma_run + "}";
        names["R_signal_pipi" + name_run] = "R^{#pi#pi" + comma_run + "}";
        names["R_ds_pipi" + name_run] = "R_{ds}^{#pi#pi" + comma_run + "}";
        names["R_Bs_pipi" + name_run] = "R_{s}^{#pi#pi" + comma_run + "}";
        names["A_Bs_pipi" + name_run] = "A_{s}^{#pi#pi" + comma_run + "}";
    }

    // Search and return
    if (names.find(par) != names.end()) {
        return names[par];
    } else {
        return par;
    }
}


// Main script
int main(int argc, char * argv[]) {

    // =====
    // Setup
    // =====
    // Get input args
    bool just_phys = true;
    bool split = true;
    bool split_obs = false;
    bool combine_runs = false;
    bool binned = false;
    bool high_stats = false;
    for (int i = 1; i < argc; i++) { 
        std::string arg = std::string(argv[i]);
        if (arg == "--binned") {
            binned = true;
        } else if (arg == "--all") {
            just_phys = false;
        } else if (arg == "--combinedRuns") {
            combine_runs = true;
        } else if (arg == "--combined") {
            split = false;
        } else if (arg == "--splitObs") {
            split_obs = true;
        } else if (arg == "--high_stats") {
            high_stats = true;
        } else if (arg == "--split") {
            split = true;
        } else {
            std::cout << "Unrecognised argument " << arg << std::endl;
            exit (EXIT_FAILURE);
        }
    }

    // Directory containing files
    TString dir = "";
    if (binned) dir += "/Binned/";
    if (combine_runs) dir += "/CombinedRuns/";
    if (high_stats) dir += "/high_stats/";
    if (split) dir += "/split/";
    if (split_obs) dir += "/SplitObs/";
    TString full_dir = "/data/lhcb/users/pullen/B02DKstar/toys/FitterBias/" + dir;
    std::cout << "Taking inputs from " << full_dir << std::endl;

    // Open the files
    TChain * toy_tree = new TChain("toy_tree");
    toy_tree->Add(full_dir + "/pulls_*.root");
    std::cout << "Loaded toy tree with " << toy_tree->GetEntries() << " entries." << std::endl;
    std::cout << "Entries with status = 0: " << toy_tree->GetEntries("status == 0") 
        << std::endl;
    std::cout << "Entries with covQual = 3: " << toy_tree->GetEntries("covQual == 3") 
        << std::endl;
    int good_entries = toy_tree->GetEntries("status == 0 && covQual == 3");
    std::cout << "Total good toys: " << good_entries << std::endl;
    std::cout << "Convergence rate = " << (double)good_entries/(double)toy_tree->GetEntries() * 100 
        << "%" << std::endl;

    // Make list of parameters to loop through
    TString results_filename = "../Fit_data/Results/twoAndFourBody_data";
    if (split) results_filename += "_split";
    if (combine_runs) results_filename += "_combinedRuns";
    if (split_obs) results_filename += "_splitObs";
    results_filename += ".root";
    TFile * result_file = TFile::Open(results_filename, "READ");
    RooFitResult * result = (RooFitResult*)result_file->Get("fit_result");
    RooWorkspace * wspace = (RooWorkspace*)result_file->Get("wspace");
    RooArgList vars = result->floatParsInit();
    RooRealVar * var;
    std::vector<std::string> params_list;

    // Add R_ds values to list
    if (!combine_runs) {
        params_list.push_back("R_Bs_KK_run1");
        params_list.push_back("R_Bs_KK_run2");
        params_list.push_back("R_Bs_pipi_run1");
        params_list.push_back("R_Bs_pipi_run2");
        params_list.push_back("R_Bs_pipipipi_run2");
    } else {
        params_list.push_back("R_Bs_KK");
        params_list.push_back("R_Bs_pipi");
    }

    // Add parameters from RooFitResult
    TIterator * it = vars.createIterator();
    std::map<std::string, double> init_fit_vals;
    while ((var = (RooRealVar*)it->Next())) {
        std::string fullname = var->GetName();
        std::string prefix = "pdf_params_";
        std::string shortname = fullname.substr(prefix.length(), std::string::npos);
        size_t pos = shortname.find("_blind");
        shortname = shortname.substr(0, pos);

        if (just_phys && get_name(shortname) == shortname) {
            continue;
        } else {
            params_list.push_back(shortname);
            init_fit_vals.emplace(shortname, var->getVal());
        }
    }

    // Output directory for histograms
    std::string out_dir = "Plots/";
    if (binned) out_dir += "Binned/";
    if (!just_phys) out_dir += "/All/";
    if (combine_runs) out_dir += "/CombinedRuns/";
    if (high_stats) out_dir += "/high_stats/";
    if (split) out_dir += "/split/";
    if (split_obs) out_dir += "/SplitObs/";

    // ===============
    // Make histograms
    // ===============
    // Map to hold histograms
    std::map<TString, TH1F*> hist_map;
    setPlotStyle();

    // Make canvas
    TCanvas * canvas = new TCanvas("canvas", "", 1500, 400);

    // Open output file
    std::string bfile = just_phys ? "biases" : "biases_all";
    if (combine_runs) bfile += "_combinedRuns";
    if (high_stats) bfile += "_highStats";
    if (split_obs) bfile += "_splitObs";
    bfile += ".param";
    std::ofstream bias_file(bfile);

    // Make histograms
    for (auto par : params_list) {

        // Get minimum and maximum
        // double value_min = 0;
        // double value_max = 0;
        // double error_min = 0;
        // double error_max = 0;
        // double value;
        // double error;
        // toy_tree->SetBranchAddress(("signal_final_value_" + par).c_str(), &value);
        // toy_tree->SetBranchAddress(("signal_final_error_" + par).c_str(), &error);
        // toy_tree->Draw(">>elist", "status == 0 && covQual == 3");
        // TEventList * elist = (TEventList*)gDirectory->Get("elist");
        // for (int i = 0; i < elist->GetN(); i++) {
            // toy_tree->GetEntry(elist->GetEntry(i));
            // if (i == 0) {
                // value_min = value;
                // value_max = value;
                // error_min = error;
                // error_max = error;
            // } else {
                // if (value < value_min) value_min = value;
                // if (value > value_max) value_max = value;
                // if (error < error_min) error_min = error;
                // if (error > error_max) error_max = error;
            // }
        // }

        // // Get range of variable
        // double value_buffer = (value_max - value_min);
        // double error_buffer = (error_max - error_min);
        // std::cout << "Value: " << value_max << " +/- " << value_buffer << std::endl;
        // std::cout << "Error: " << error_max << " +/- " << error_buffer << std::endl;

        // Check limits of pulls
        // double pull_max = 10;
        // double pull_min = -1 * pull_max;
        // int bins_pulls = n_bins;
        // toy_tree->Draw(("signal_pull_" + par + ">>temp").c_str());
        // TH1F * temp_hist = (TH1F*)gDirectory->Get("temp");

        // Make histograms: value, error, pull
        // TH1F * hist_value = new TH1F(("hist_value_" + par).c_str(), "", n_bins,
                // value_min - value_buffer, value_max + value_buffer);
        // TH1F * hist_error = new TH1F(("hist_error_" + par).c_str(), "", n_bins,
                // error_min - error_buffer, error_max + error_buffer);
        // TH1F * hist_pulls = new TH1F(("hist_pulls_" + par).c_str(), "", bins_pulls,
                // pull_min, pull_max);

        // Fill histograms
        TCut cut = "status == 0 && covQual == 3";
        if (par == "R_signal_pipi") cut += "signal_final_error_R_signal_pipi < 0.3";
        toy_tree->Draw(("signal_final_value_" + par + ">>hist_value_" + par).c_str(), cut);
        toy_tree->Draw(("signal_final_error_" + par + ">>hist_error_" + par).c_str(), cut);
        toy_tree->Draw(("signal_pull_" + par + ">>hist_pulls_" + par).c_str(), cut);
        TH1F * hist_value = (TH1F*)gDirectory->Get(("hist_value_" + par).c_str());
        TH1F * hist_error = (TH1F*)gDirectory->Get(("hist_error_" + par).c_str());
        TH1F * hist_pulls = (TH1F*)gDirectory->Get(("hist_pulls_" + par).c_str());
        canvas->Clear();

        // Plot values
        hist_value->SetLineWidth(1);
        hist_value->SetLineColor(kBlack);
        hist_value->SetMarkerSize(0);
        hist_value->GetXaxis()->SetTitle((get_name(par) + " value").c_str());
        hist_value->GetXaxis()->SetTitleOffset(1.1);
        hist_value->SetStats(false);
        canvas->Divide(3, 1);
        canvas->cd(1);
        hist_value->Draw();
        canvas->Update();

        // Draw line at initial value 
        double init_value = toy_tree->GetMinimum(("signal_init_value_" + par).c_str());
        double value_y_max = canvas->GetPad(1)->GetUymax();
        TLine * value_line = new TLine(init_value, 0, init_value, value_y_max);
        value_line->SetLineColor(kRed);
        value_line->SetLineStyle(2);
        value_line->Draw();
        gPad->RedrawAxis();

        // Plot errors
        hist_error->SetLineWidth(1);
        hist_error->SetLineColor(kBlack);
        hist_error->GetXaxis()->SetTitle((get_name(par) + " error").c_str());
        hist_error->GetXaxis()->SetTitleOffset(1.1);
        hist_error->SetStats(false);
        canvas->cd(2);
        hist_error->Draw();
        canvas->Update();

        // Draw line at initial error
        double init_error;
        if (par.find("R_Bs") == std::string::npos) {
            init_error = toy_tree->GetMinimum(("signal_init_error_" + par).c_str());
        } else {
            RooFormulaVar * var = (RooFormulaVar*)wspace->arg((
                        "pdf_params_" + par).c_str());
            init_error = var->getPropagatedError(*result);
        }
        double error_y_max = gPad->GetUymax();
        TLine * error_line = new TLine(init_error, 0, init_error, error_y_max);
        error_line->SetLineColor(kRed);
        error_line->SetLineStyle(2);
        error_line->Draw();
        gPad->RedrawAxis();

        // Plot pulls
        hist_pulls->SetLineWidth(1);
        hist_pulls->SetLineColor(kBlack);
        hist_pulls->GetXaxis()->SetTitle("Pull");
        hist_pulls->GetXaxis()->SetTitleOffset(1.1);
        hist_pulls->GetYaxis()->SetRangeUser(0, hist_pulls->GetMaximum() * 1.6);
        canvas->cd(3);
        hist_pulls->Draw("E");

        // Fit the pull histogram with a Gaussian
        if (!just_phys) {

            // Just plot pulls
            hist_pulls->SetStats(false);
            hist_pulls->Draw("E SAME");

        } else {

            if (hist_pulls->Integral() != 0) {

                // Perform fit
                hist_pulls->Fit("gaus");
                TF1 * gauss_fit = hist_pulls->GetFunction("gaus");
                gauss_fit->SetLineColor(862);
                gauss_fit->SetLineWidth(1);
                gauss_fit->Draw("C SAME");

                // Draw pulls
                hist_pulls->SetStats(false);
                hist_pulls->Draw("E SAME");

                // Make stats box
                TPaveText * stats = new TPaveText(0.55, 0.7, 0.8, 0.9, "NDC");
                stats->SetLineColor(0);
                stats->SetFillColor(0);
                stats->SetFillStyle(0);
                stats->SetShadowColor(0);
                stats->SetCornerRadius(0);
                stats->SetBorderSize(0);
                std::stringstream mu_stream;
                mu_stream << "#mu = ";
                mu_stream << std::setprecision(3) << gauss_fit->GetParameter("Mean"); 
                mu_stream << " #pm " << std::setprecision(2) << gauss_fit->GetParError(gauss_fit->GetParNumber("Mean"));
                stats->AddText(mu_stream.str().c_str());
                std::stringstream sigma_stream;
                sigma_stream << "#sigma = ";
                sigma_stream << std::setprecision(4) << gauss_fit->GetParameter("Sigma"); 
                sigma_stream << " #pm " << std::setprecision(2) << gauss_fit->GetParError(gauss_fit->GetParNumber("Sigma"));
                stats->AddText(sigma_stream.str().c_str());
                TVirtualPad * pad = canvas->cd(3);
                stats->Draw();
                pad->Modified();
                pad->Update();

                // Write to file
                bias_file << par << " " << gauss_fit->GetParameter("Mean")
                    << " " << gauss_fit->GetParError(gauss_fit->GetParNumber("Mean"))
                    << std::endl;

            } else {
                std::cout << "Could not fit pull for variable " << par <<
                    std::endl;
            }
        }

        // Draw extra histogram with failed toys if plotting all
        // if (!just_phys) {
            // // TH1F * hist_bad = new TH1F(("hist_bad_" + par).c_str(), "", n_bins,
                    // // value_min - value_buffer, value_max + value_buffer);
            // canvas->cd(1);
            // toy_tree->Draw(("signal_final_value_" + par + ">>hist_bad_" + par).c_str(),
                    // "status != 0 || covQual != 3");
            // TH1F * hist_bad = (TH1F*)gDirectory->Get(("hist_bad_" + par).c_str());
            // hist_bad->SetFillColorAlpha(kRed, 0.5);
            // hist_bad->Draw("HIST SAME");
        // }


        // Save the canvas
        canvas->SaveAs((out_dir + par + ".pdf").c_str());

    } // End loop over parameters
    if (just_phys) bias_file.close();

}
