#include <cmath>
#include <iostream>
#include <string>

#include "TCanvas.h"

#include "RooAbsReal.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooDataHist.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooFitResult.h"
#include "RooMCStudy.h"
#include "RooMinuit.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "PlotStyle.hpp"

#include "ShapeMaker.hpp"

// =============================================
// Script to scan likelihood in piK signal yield
// =============================================
int main(int argc, char * argv[]) {

    // Name of file with combined data fit
    std::string result_file = "../Results/twoBody_2011:2012:2015:2016_"
        "combined_binned.root";

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);

    // Make ShapeMaker
    ShapeMaker * sm = new ShapeMaker("Y", Bd_M);

    // Get toy PDF
    RooSimultaneous * toyPdf = sm->makeGenerationPdf(result_file);
    int expectedEvents = toyPdf->expectedEvents(*sm->getCategory());

    // Generate dataset
    RooDataSet * data = toyPdf->generate(RooArgList(*Bd_M, *sm->getCategory()), 
            expectedEvents);

    // Make binned dataset
    RooDataHist * data_hist = (RooDataHist*)data->binnedClone();

    // Make fitting PDFs with and without floating piK yield
    RooSimultaneous * fitPdf = sm->makeFitPdf(false);
    RooSimultaneous * fitPdf_noPiK = sm->makeZeroYieldPdf();

    // Fit and get RooFitResult
    RooFitResult * result_floating = fitPdf->fitTo(*data_hist, RooFit::Save(),
            RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    RooFitResult * result_noPiK = fitPdf_noPiK->fitTo(*data_hist, RooFit::Save(),
            RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));

    // Print results
    double nll_floating = result_floating->minNll();
    double nll_noPiK = result_noPiK->minNll();
    std::cout << "Ratio between piK and Kpi obtained: " << 
        sm->getFitVariable("R_piK_vs_Kpi")->getVal() << std::endl;
    std::cout << "Minimum NLL with free piK yield: " << nll_floating << std::endl;
    std::cout << "Minimum NLL with zero piK yield: " << nll_noPiK << std::endl;
    std::cout << "Significance: " << sqrt(2 * (nll_noPiK - nll_floating)) <<
            std::endl;

    return 0;
}

