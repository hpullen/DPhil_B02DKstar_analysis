#include "TRandom.h"

#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooFitResult.h"
#include "RooRandom.h"
#include "RooDataHist.h"

// ========================================
// Script for testing ParameterManager code
// ========================================
#include <iostream>

#include "ToySignificanceFitter.hpp"
// #include "Plotter.hpp"
// #include "SimpleToyMaker.hpp"
// #include "SimplePdfMaker.hpp"
// #include "ToyFitter.hpp"

int main(int argc, char * argv[]) {

    ToySignificanceFitter * tf = new ToySignificanceFitter();
    tf->PerformFits("results.root", 1);
    delete tf;

    // // Make mass variable
    // RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);
    // int binWidth = 8;
    // double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    // Bd_M->setBins(nBins);

    // // Make category
    // RooCategory * cat = new RooCategory("modes", "");
    // cat->defineType("Kpi");
    // cat->defineType("piK");

    // // Generate toy
    // SimpleToyMaker * tm = new SimpleToyMaker(Bd_M, cat);
    // ToyFitter * tf = new ToyFitter(tm);

    // // Fit PDF
    // SimplePdfMaker * pdf_signal = new SimplePdfMaker("signal", Bd_M, cat);
    // SimplePdfMaker * pdf_null = new SimplePdfMaker("null", Bd_M, cat, Hypothesis::Null);
    // tf->AddFitPdf(pdf_signal);
    // tf->AddFitPdf(pdf_null);
    // tf->PerformFits("results.root", 10);


    // delete tf;
}
