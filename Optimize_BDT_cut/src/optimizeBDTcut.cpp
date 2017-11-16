#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TIterator.h"

#include "RooFitResult.h"
#include "RooArgList.h"
#include "RooRealVar.h"

#include "PlotStyle.hpp"

// =====================================================
// Script to make a 2D plot of an error vs. two BDT cuts
// =====================================================
int main(int argc, char * argv[]) {

    // Set up custom plotting
    setPlotStyle();
    
    // Check input args
    if (argc < 4) {
        std::cout << "Usage: ./OptimizeBDTcut <mode1> <mode2> <Vars to plot>..."
            << std::endl;
        return -1;
    }

    // Get input args
    std::string mode1 = argv[1];
    std::string mode2 = argv[2];
    std::vector<std::string> vars_to_plot;
    for (int i = 3; i < argc; i++) {
        vars_to_plot.push_back(argv[i]);
    }

    // List of available cuts as strings
    std::vector<std::string> cuts = {"0.2", "0.3", "0.4", "0.5", "0.6", "0.7",
        "0.8", "0.9"};
    std::string standard_cut = "0.5";

    // Make histograms to hold results
    std::map<std::string, TH2F *> hists;
    for (auto var : vars_to_plot) {
        hists[var] = new TH2F(("hist_" + var).c_str(), "", 8, 0.15, 0.95, 
                8, 0.15, 0.95);
    }

    // Set up cut values for each mode
    std::map<std::string, std::string> cut_values;
    cut_values["Kpi"] = standard_cut;
    cut_values["piK"] = standard_cut;
    cut_values["KK"] = standard_cut;
    cut_values["pipi"] = standard_cut;

    // Location of RooFitResults
    std::string file_base = "/home/pullen/analysis/B02DKstar/Fit_data/Results/"
        "BDT_studies/twoBody_2011:2012:2015:2016_split_binned_";

    // Loop through cut values
    for (auto cut1 : cuts) {
        for (auto cut2 : cuts) {

            // Set cuts for each mode
            for (auto mode : cut_values) {
                cut_values[mode.first] = standard_cut;
                if (mode.first == mode1) cut_values[mode.first] = cut1; 
                else if (mode.first == mode2) cut_values[mode.first] = cut2;
            }

            // Open RooFitResult
            std::string filename = file_base + cut_values["Kpi"] + "_" +
                cut_values["piK"] + "_" + cut_values["KK"] + "_" + 
                cut_values["pipi"] + ".root";
            TFile * file = TFile::Open(filename.c_str(), "READ");
            RooFitResult * r = (RooFitResult*)file->Get("fit_result");

            // Extract variables
            RooArgList vars = r->floatParsFinal();
            TIterator * it = vars.createIterator();
            RooRealVar * any_var;

            // Loop through variables and store those of interest
            while ((any_var = (RooRealVar*)it->Next())) {

                for (auto var : vars_to_plot) {
                    if (var == any_var->GetName()) {
                        hists[var]->Fill(std::stod(cut1), std::stod(cut2),
                                any_var->getError());
                        std::cout << "Error: " << any_var->getError() << std::endl;
                    }
                }
            }
        }
    }

    // Plot the histograms
    for (auto hist : hists) {

        // Make canvas
        TCanvas * canvas = new TCanvas("canvas", "", 500, 400);
        canvas->cd();

        // Prepare histogram and draw
        hist.second->SetStats(false);
        hist.second->SetMinimum(0.001);
        hist.second->SetMaximum(0.005);
        hist.second->GetXaxis()->SetTitle((mode1 + " BDT cut").c_str());
        hist.second->GetYaxis()->SetTitle((mode2 + " BDT cut").c_str());
        hist.second->Draw("COLZ");

        // Save the plot
        canvas->SaveAs(("Plots/" + hist.first + "_" + mode1 + "_cut_vs_"
                    + mode2 + "_cut.pdf").c_str());
        delete canvas;
    }

    std::cout << "Successfully produced " << hists.size() << " plots." << std::endl;
    return -1;

}
