#include <iostream>
#include <string>

#include "TCanvas.h"

#include "RooDataSet.h"
#include "RooPlot.h"
#include "RooAbsReal.h"
#include "RooMinuit.h"
#include "RooDataHist.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooFitResult.h"
#include "RooSimultaneous.h"
#include "RooMCStudy.h"

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

    // Make fitting PDF
    RooSimultaneous * fitPdf = sm->makeFitPdf(false);

    // Make likelihood
    RooAbsReal * nll = fitPdf->createNLL(*data_hist, RooFit::NumCPU(8));
    RooMinuit(*nll).migrad();

    // Profile likelihood in R_piK_vs_Kpi
    RooAbsReal * profile_R = 
        nll->createProfile(RooArgSet(*sm->getFitVariable("R_piK_vs_Kpi")));
    
    // Plot likelihood function
    setPlotStyle();
    RooPlot * frame = ((RooRealVar*)sm->getFitVariable("R_piK_vs_Kpi"))->frame();
    profile_R->plotOn(frame);
    TCanvas * canvas = new TCanvas("canvas", "", 500, 400);
    frame->Draw();
    canvas->SaveAs("../Plots/likelihood.pdf");

    return 0;
}

