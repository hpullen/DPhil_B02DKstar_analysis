#include "TRandom.h"

#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooFitResult.h"
#include "RooRandom.h"
#include "RooDataHist.h"

// ======================================================
// Script for running toy studies to look for fitter bias
// ======================================================
#include <iostream>

#include "Plotter.hpp"
#include "PlotStyle.hpp"
#include "TwoAndFourBodyToyMaker.hpp"
#include "TwoAndFourBodyPdfMaker.hpp"
#include "ToyFitter.hpp"

int main(int argc, char * argv[]) {

    // Get a number to ID the file
    if (arc != 2) {
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
    cat->defineType("Kpi");
    cat->defineType("piK");
    cat->defineType("KK");
    cat->defineType("pipi");
    cat->defineType("Kpipipi");
    cat->defineType("piKpipi");
    cat->defineType("pipipipi");

    // Generate toy
    TwoAndFourBodyToyMaker * tm = new TwoAndFourBodyToyMaker(Bd_M, cat,
            "Results/FitterBias/pulls_" + number + ".root");
    ToyFitter * tf = new ToyFitter(tm);

    // Fit PDF
    TwoAndFourBodyPdfMaker * pdf_signal = new TwoAndFourBodyPdfMaker("signal", 
            Bd_M, cat, false);
    tf->AddFitPdf(pdf_signal);
    tf->PerformFits("results.root", 10);

    delete tf;
}
