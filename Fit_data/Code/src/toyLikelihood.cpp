#include <cmath>
#include <iostream>
#include <string>

#include "TCanvas.h"

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

#include "PlotStyle.hpp"

#include "Plotter.hpp"
#include "ShapeMaker.hpp"

// =============================================
// Script to scan likelihood in piK signal yield
// =============================================
int main(int argc, char * argv[]) {

    // Check args
    if (argc > 4) {
        std::cout << "Usage: ./ToyLikelihood "
            "(--tight-pid/--previous-analysis/--with-2017/--tight-bdt --save "
            "(name number))" 
            << std::endl;
        return -1;
    }

    // Get option
    bool use_tight_cut = false;
    bool use_prev_ana = false;
    bool use_2017 = false;
    bool use_tight_bdt = false;
    bool save = false;
    bool plot = true;
    std::string name;
    std::string number;
    if (argc > 1) {
        std::string option = std::string(argv[1]);
        if (option == "--tight-pid") use_tight_cut = true;
        else if (option == "--previous-analysis") use_prev_ana = true;
        else if (option == "--with-2017") use_2017 = true;
        else if (option == "--tight-bdt") use_tight_bdt = true;
        else if (option == "--save") {
            save = true;
            plot = false;
            name = std::string(argv[2]);
            number = std::string(argv[3]);
            use_tight_cut = true;
        }
        else {
            std::cout << "Unknown option! Aborting." << std::endl;
            return -1;
        }
    }

    // Name of file with combined data fit
    std::string result_file;
    std::string plotname;
    if (use_tight_cut) {
        std::cout << "Using tighter PID cut" << std::endl;
        result_file = "../Results/twoBody_tightPID.root";
        plotname = "toy_tightPID";
    } else {
        std::cout << "Using looser cut" << std::endl;
        result_file = "../Results/twoBody_2011:2012:2015:2016_"
        "combined_binned.root";
        plotname = "toy_loosePID";
    }
    if (use_prev_ana) {
        std::cout << "Using previous analysis yields" << std::endl;
        plotname = "toy_previousAnalysis";
    }
    if (use_2017) {
        std::cout << "Using estimatd 2017 contributions" << std::endl;
        plotname = "toy_with2017";
    }
    if (use_tight_bdt) {
        std::cout << "Using tighter BDT cut" << std::endl;
        result_file = "../Results/BDT_studies/twoBody_2011:2012:2015:2016_"
            "combined_binned_0_0_0.5_0.5.root";
        plotname = "toy_tightBDT";
    }

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);

    // Make ShapeMaker
    ShapeMaker * sm = new ShapeMaker("Y", Bd_M);

    // Get toy PDF
    RooSimultaneous * toyPdf;
    if (use_prev_ana) {
        toyPdf = sm->makePreviousAnalysisPdf();
    } else if (use_2017) {
        toyPdf = sm->make_2017_pdf();
    } else {
        toyPdf = sm->makeGenerationPdf(result_file);
    }
    int expectedEvents = toyPdf->expectedEvents(*sm->getCategory());

    // Generate dataset
    RooDataSet * data = toyPdf->generate(RooArgList(*Bd_M, *sm->getCategory()),
            expectedEvents);

    // Make binned dataset
    RooDataHist * data_hist = (RooDataHist*)data->binnedClone();

    // Separate categories of data for plotting
    std::map<std::string, RooDataSet*> dataMap;
    dataMap["Kpi"] = (RooDataSet*)data->reduce("category == category::Kpi");
    dataMap["piK"] = (RooDataSet*)data->reduce("category == category::piK");
    dataMap["KK"] = (RooDataSet*)data->reduce("category == category::KK");
    dataMap["pipi"] = (RooDataSet*)data->reduce("category == category::pipi");

    // Fit with free yield
    RooSimultaneous * fitPdf = sm->makeFitPdf(false);
    RooFitResult * result_floating = fitPdf->fitTo(*data_hist, RooFit::Save(),
            RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    if (plot) {
        result_floating->Print("v");
        sm->saveFitHistograms("../Histograms/" + plotname + "_freePiK.root", dataMap,
                false);
    }
    if (save) {
        std::string filename1 = "../Results/Toy_significance/" + name + "_" + 
            number + "_floating.root";
        result_floating->SaveAs(filename1.c_str());
    }
    double nll_floating = result_floating->minNll();

    // Fit with piK yield fixed to zero
    RooSimultaneous * fitPdf_noPiK = sm->makeZeroYieldPdf();
    RooFitResult * result_noPiK = fitPdf_noPiK->fitTo(*data_hist, RooFit::Save(),
            RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    if (plot) {
        result_noPiK->Print("v");
        sm->saveFitHistograms("../Histograms/" + plotname + "_zeroPiK.root", dataMap,
                false);
    }
    if (save) {
        std::string filename2 = "../Results/Toy_significance/" + name + "_" + 
            number + "_fixed.root";
        result_floating->SaveAs(filename2.c_str());
    }
    double nll_noPiK = result_noPiK->minNll();

    // Plot
    if (plot) {
        Plotter * plotter = new Plotter(false);
        plotter->plotFourModeFitsCombined("../Histograms/" + plotname + "_freePiK.root",
                plotname + "_freePiK", "");
        plotter->plotFourModeFitsCombined("../Histograms/" + plotname + "_zeroPiK.root",
                plotname + "_zeroPiK", "");
    }

    // Print results
    std::cout << "Ratio between piK and Kpi obtained: " <<
        sm->getFitVariable("R_piK_vs_Kpi")->getVal() << std::endl;
    std::cout << "Minimum NLL with free piK yield: " << nll_floating << std::endl;
    std::cout << "Minimum NLL with zero piK yield: " << nll_noPiK << std::endl;
    std::cout << "Significance: " << sqrt(2 * (nll_noPiK - nll_floating)) <<
            std::endl;

    return 0;
}

