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
    if (argc != 2) {
        std::cout << "Usage: ./FitterBias <run-number>" << std::endl;
        return -1;
    }
    std::string number = std::string(argv[1]);

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);

    // Make category
    RooCategory * cat = new RooCategory("modes", "");
    // cat->defineType("Kpi");
    // cat->defineType("Kpipipi");
    for (TString run : {"_run1", "_run2"}) {
        cat->defineType("Kpi" + run);
        // cat->defineType("piK" + run);
        // cat->defineType("KK" + run);
        // cat->defineType("pipi" + run);
        cat->defineType("Kpipipi" + run);
        // cat->defineType("piKpipi" + run);
    }
    // cat->defineType("pipipipi_run2");

    // Generate toy
    ToyPdfMaker * tm = new ToyPdfMaker(Bd_M, cat, "Results/twoAndFourBody_data.root");
    ToyFitter * tf = new ToyFitter(tm);

    // Fit PDF
    DataPdfMaker * pdf_signal = new DataPdfMaker("signal", 
            Bd_M, cat, true);
    tf->AddFitPdf(pdf_signal);
    // tf->PerformFits("Results/FitterBias/pulls_" + number + ".root", 1);
    tf->PerformFits("Results/FitterBias/pulls_" + number + ".root", 10);

    delete tf;
}
