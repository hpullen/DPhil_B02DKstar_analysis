// ======================================================
// Script for running toy studies to look for fitter bias
// ======================================================
#include <iostream>

#include "TRandom.h"

#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooFitResult.h"
#include "RooRandom.h"
#include "RooDataHist.h"

#include "Plotter.hpp"
#include "PlotStyle.hpp"
#include "ToyPdfMaker.hpp"
#include "DataPdfMaker.hpp"
#include "ToyFitter.hpp"

int main(int argc, char * argv[]) {

    // Get a number to ID the file
    if (argc < 2) {
        std::cout << "Usage: ./FitterBias <run-number> (--high_stats --binned"
            " --single --fine_bins --split --combineRuns --splitObs)" 
            << std::endl;
        return -1;
    }
    std::string number = std::string(argv[1]);
    bool high_stats = false;
    bool single = false;
    bool fine_bins = false;
    bool split = false;
    bool limited_modes = false;
    bool binned = false;
    bool combine_runs = false;
    bool split_obs = false;
    std::vector<std::string> limited_modes_to_use;
    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            std::string opt = argv[i];
            if (opt == "--high_stats") {
                high_stats = true;
                std::cout << "High stats toy" << std::endl;
            } else if (opt == "--single") {
                single = true;
                std::cout << "One toy only" << std::endl;
            } else if (opt == "--fine_bins") {
                fine_bins = true;
                std::cout << "Finer binning" << std::endl;
            } else if (opt == "--split" || opt == "-s") {
                split = true;
                std::cout << "Splitting by flavour" << std::endl;
            } else if (opt == "--combineRuns" || opt == "-c") {
                combine_runs = true;
                std::cout << "Combining runs" << std::endl;
            } else if (opt == "--splitObs") {
                split_obs = true;
                std::cout << "Splitting observables by run" << std::endl;
            } else if (opt == "--modes") {
                limited_modes = true;
                std::cout << "Using modes: ";
                int j = i + 1; 
                while (j < argc) {
                    std::string jarg = argv[j];
                    if (jarg.find("--") != std::string::npos) {
                        break;
                    } else {
                        std::cout << jarg << " ";
                        limited_modes_to_use.push_back(jarg);
                    }
                    j++;
                }
                i = j - 1;
                std::cout << std::endl;
            } else if (opt == "--binned" || opt == "-b") {
                binned = true;
                std::cout << "Binned fit" << std::endl;
            } else {
                std::cout << "Unrecognised option: " << opt << std::endl;
                return -1;
            }
        }
    }
    if (split_obs && combine_runs) {
        std::cout << "Error: Cannot split observables in a combined run fit!" << std::endl;
        return -1;
    }

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);
    double binWidth = 8;
    if (fine_bins) binWidth = 0.8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    std::cout << "Setting N bins to " << nBins << std::endl;
    Bd_M->setBins(nBins);

    // Make category
    RooCategory * cat = new RooCategory("modes", "");
    std::vector<TString> flavs = {""};
    if (split) flavs = {"_plus", "_minus"};
    std::vector<TString> runs = {""};
    if (!combine_runs) runs = {"_run1", "_run2"};
    for (TString flav : flavs) {
        if (!limited_modes) {
            if (combine_runs) {
                cat->defineType("Kpi" + flav);
                cat->defineType("piK" + flav);
                cat->defineType("KK" + flav);
                cat->defineType("pipi" + flav);
                cat->defineType("Kpipipi" + flav);
                cat->defineType("piKpipi" + flav);
            } else {
                for (TString run : {"_run1", "_run2"}) {
                    cat->defineType("Kpi" + run + flav);
                    cat->defineType("piK" + run + flav);
                    cat->defineType("KK" + run + flav);
                    cat->defineType("pipi" + run + flav);
                    cat->defineType("Kpipipi" + run + flav);
                    cat->defineType("piKpipi" + run + flav);
                }
                cat->defineType("pipipipi_run2" + flav);
            }
        } else {
            for (auto const & mode : limited_modes_to_use) {
                for (TString run : {"_run1", "_run2"}) {
                    if (run != "_run2" && mode == "pipipipi") continue;
                    cat->defineType(mode + run + flav);
                }
            }
        }
    }

    // Generate toy
    std::string results_file = "Results/twoAndFourBody_data"; 
    if (split) results_file += "_split";
    if (combine_runs) results_file += "_combinedRuns";
    // if (binned) results_file += "_binned";
    if (split_obs) results_file += "_splitObs";
    results_file += ".root";
    std::cout << "Taking input from " << results_file << std::endl;
    ToyPdfMaker * tm = new ToyPdfMaker(Bd_M, cat, results_file, high_stats, 
            combine_runs, split_obs);
    ToyFitter * tf = new ToyFitter(tm, binned, combine_runs, split_obs);

    // Fit PDF
    DataPdfMaker * pdf_signal = new DataPdfMaker("signal", Bd_M, cat, 
            false, split_obs);
    std::string extra = binned ? "/Binned/" : "";
    if (combine_runs) extra += "CombinedRuns/";
    if (high_stats) extra += "/high_stats/";
    if (split) extra += "/split/";
    if (split_obs) extra += "/SplitObs/";
    tf->AddFitPdf(pdf_signal);
    if (single) {
        tf->PerformFits("Results/FitterBias/test_" + number + ".root", 1);
    } else {
        std::cout << "WILL PERFORM 10 FITS" << std::endl;
        tf->PerformFits("/data/lhcb/users/pullen/B02DKstar/toys/FitterBias/"
                + extra + "pulls_" + number + ".root", 10);
    }

    delete tf;
}
