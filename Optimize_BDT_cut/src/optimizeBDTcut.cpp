#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

#include "TSystem.h"
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
    if (argc != 4 && argc != 6) {
        std::cout << "Usage: ./OptimizeBDTcut <mode1> <mode2> <Var to plot> "
            "(<min> <max>)" << std::endl;
        return -1;
    }

    // Get input args
    std::string mode1 = argv[1];
    std::string mode2 = argv[2];
    std::string var_to_plot = argv[3];
    double min;
    double max;
    bool set_range = false;
    if (argc == 6) {
        min = atof(argv[4]);
        max = atof(argv[5]);
        set_range = true;
    }

    // List of available cuts as strings
    std::vector<std::string> cuts = {"-0.3", "-0.2", "-0.1", "0", "0.1", "0.2", 
        "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9"};
    std::string standard_cut = "0.5";

    // Make histogram to hold results
    TH2F * hist = new TH2F(("hist_" + var_to_plot).c_str(), "", 13, -0.35, 0.95, 13, -0.35, 0.95);

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

            // Check file existance
            std::string filename = file_base + cut_values["Kpi"] + "_" +
                cut_values["piK"] + "_" + cut_values["KK"] + "_" + 
                cut_values["pipi"] + ".root";
            Long_t * id, * size, * flags, * modtime;
            if (gSystem->GetPathInfo(filename.c_str(), id, size, flags, modtime)) {
                    std::cout << "File " << filename << " does not exist" << 
                    std::endl;
                    continue;
                }

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
                    hist->Fill(std::stod(cut1), std::stod(cut2),
                            var->getError());
                    std::cout << "Uncertainty: " << var->getError() << std::endl;
                }
            }
        }
    }

    // Plot the histogram
    // Make canvas
    TCanvas * canvas = new TCanvas("canvas", "", 500, 400);
    canvas->cd();

    // Set range of histogram if desired
    if (set_range) {
        hist->SetMinimum(min);
        hist->SetMaximum(max);
    }

    // Prepare histogram and draw
    hist->SetStats(false);
    hist->GetXaxis()->SetTitle((mode1 + " BDT cut").c_str());
    hist->GetYaxis()->SetTitle((mode2 + " BDT cut").c_str());
    hist->Draw("COLZ");

    // Save the plot
    canvas->SaveAs(("Plots/" + var_to_plot + "_" + mode1 + "_cut_vs_"
                + mode2 + "_cut.pdf").c_str());
    delete canvas;

    return -1;

}
