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

#include "SimpleToyMaker.hpp"
#include "SimplePdfMaker.hpp"
#include "ToyFitter.hpp"

int main(int argc, char * argv[]) {

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);

    // Make category
    RooCategory * cat = new RooCategory("modes", "");
    cat->defineType("Kpi");
    cat->defineType("piK");

    // Generate toy
    SimpleToyMaker * tm = new SimpleToyMaker(Bd_M, cat);
    ToyFitter * tf = new ToyFitter(tm, "results.root");

    // Fit PDF
    SimplePdfMaker * pm = new SimplePdfMaker(Bd_M, cat);
    tf->AddFitPdf(pm);
    tf->PerformFits(0);

    delete tf;
    delete tm;

}
