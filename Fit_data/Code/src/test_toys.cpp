#include <cmath>
#include <iostream>
#include <string>

#include "TCanvas.h"
#include "TRandom.h"
#include "TTree.h"
#include "TIterator.h"
#include "TString.h"
#include "TFile.h"

#include "RooAbsReal.h"
#include "RooArgSet.h"
#include "RooMsgService.h"
#include "RooCategory.h"
#include "RooDataHist.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooMCStudy.h"
#include "RooMinuit.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooRandom.h"

#include "PlotStyle.hpp"

#include "Plotter.hpp"
#include "ShapeMaker.hpp"
int main(int argc, char * argv[]) {

    // Setup
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);
    ShapeMaker * sm = new ShapeMaker("Y", Bd_M, false);

    // Generate toy
    RooSimultaneous * toyPdf = sm->makePreviousAnalysisPdf();
    int expectedEvents = toyPdf->expectedEvents(RooArgSet(*sm->getCategory()));
    TRandom * rand = new TRandom();
    rand->SetSeed();
    RooRandom::setRandomGenerator(rand);
    std::cout << "Random: " << RooRandom::integer(10) << std::endl;
    RooDataSet * data = toyPdf->generate(RooArgList(*Bd_M, *sm->getCategory()),
            expectedEvents);
    RooDataHist * data_hist = (RooDataHist*)data->binnedClone();

    // Separate categories of data for plotting
    std::map<std::string, RooDataSet*> dataMap;
    dataMap["Kpi"] = (RooDataSet*)data->reduce("category == category::Kpi");
    dataMap["piK"] = (RooDataSet*)data->reduce("category == category::piK");
    dataMap["KK"] = (RooDataSet*)data->reduce("category == category::KK");
    dataMap["pipi"] = (RooDataSet*)data->reduce("category == category::pipi");

    // Make fit PDF with only free piK component
    RooSimultaneous * fitPdf = sm->makePreviousAnalysisPdf(FixedPdfOpt::FreeRatioOnly);

    // Fit and print
    // RooFitResult * r = fitPdf->fitTo(*data_hist, RooFit::Save(),
            // RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
            // RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    // r->Print("v");

    // Save
    sm->saveFitHistograms("../Histograms/test_toy_free_ratio_only_no_fitTo.root", dataMap,
            false);

    // Plot
    Plotter * plotter = new Plotter(false);
    plotter->plotFourModeFitsCombined("../Histograms/test_toy_free_ratio_only_no_fitTo.root",
            "test_toy_free_ratio_only_no_fitTo", "");

}

