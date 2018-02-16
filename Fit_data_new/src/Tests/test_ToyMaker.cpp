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
    TRandom * rand = new TRandom;
    rand->SetSeed();
    RooRandom::setRandomGenerator(rand);
    RooDataHist * data = tm->Shape()->generateBinned(RooArgList(*Bd_M, *cat),
            tm->Shape()->expectedEvents(*cat));

    // Fit to toy
    SimplePdfMaker * pm = new SimplePdfMaker(Bd_M, cat);
    RooFitResult * result = pm->Shape()->fitTo(*data, RooFit::Save(),
                RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    result->Print("v");
    pm->SaveHistograms("test.root");

    delete tm;

}
