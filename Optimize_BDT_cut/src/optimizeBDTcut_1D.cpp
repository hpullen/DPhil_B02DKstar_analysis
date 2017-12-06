#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

#include "TSystem.h"
#include "TAttMarker.h"
#include "TGraphPainter.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TIterator.h"
#include "TGraph.h"
#include "TAxis.h"

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
    if (argc != 3) {
        std::cout << "Usage: ./OptimizeBDTcut <mode> <Var to plot> " << std::endl;
        return -1;
    }

    // Get input args
    std::string mode1 = argv[1];
    std::string var_to_plot = argv[2];

    // List of available cuts as strings
    std::vector<std::string> cuts = {"-0.3", "-0.2", "-0.1", "0", "0.1", "0.2", 
        "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9"};
    std::string standard_cut = "0.5";

    // Make histogram to hold results
    TGraph * graph = new TGraph();

    // Set up cut values for each mode
    std::map<std::string, std::string> cut_values;
    cut_values["Kpi"] = standard_cut;
    cut_values["KK"] = standard_cut;
    cut_values["pipi"] = standard_cut;

    // Location of RooFitResults
    std::string file_base = "/home/pullen/analysis/B02DKstar/Fit_data/Results/"
        "BDT_studies/twoBody_2011:2012:2015:2016_split_binned_";

    // Loop through cut values
    int n_bad = 0;
    int count = 0;
    for (auto cut1 : cuts) {

            // Set cuts for each mode
            for (auto mode : cut_values) {
                cut_values[mode.first] = standard_cut;
                if (mode.first == mode1) cut_values[mode.first] = cut1; 
            }

            // Filename
            std::string filename = file_base + cut_values["Kpi"] + "_" +
                cut_values["Kpi"] + "_" + cut_values["KK"] + "_" + 
                cut_values["pipi"] + ".root";

            // Open RooFitResult
            TFile * file = TFile::Open(filename.c_str(), "READ");
            RooFitResult * r = (RooFitResult*)file->Get("fit_result");

            // Extract variables
            RooArgList vars = r->floatParsFinal();
            TIterator * it = vars.createIterator();
            RooRealVar * var;

            // Loop through variables and store those of interest
            while ((var = (RooRealVar*)it->Next())) {
                if (var_to_plot == var->GetName()) {
                    std::cout << "Uncertainty: " << var->getError() << std::endl;
                    std::cout << "Status: " << r->status() << std::endl;
                    if (r->status() == 0) {
                        graph->SetPoint(count, std::stod(cut1), var->getError());
                        count++;
                    } else {
                        n_bad++;
                    }
                }
            }
        }
    std::cout << "Ignored " << n_bad << " bad fit results." << std::endl;

    // Plot the histogram
    // Make canvas
    TCanvas * canvas = new TCanvas("canvas", "", 500, 400);
    canvas->cd();

    // Prepare histogram and draw
    graph->GetXaxis()->SetTitle((mode1 + " BDT cut").c_str());
    graph->GetYaxis()->SetTitle(var_to_plot.c_str());
    graph->SetMarkerStyle(2);
    graph->SetMarkerSize(2);
    graph->SetMarkerColor(kBlack);
    graph->Draw("ALP");

    // Save the plot
    canvas->SaveAs(("Plots/" + var_to_plot + "_" + mode1 + "_cut.pdf").c_str());
    delete canvas;

    return -1;

}
