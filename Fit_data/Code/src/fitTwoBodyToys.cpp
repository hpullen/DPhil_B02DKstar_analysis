#include <iostream>
#include <sstream>

#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooFitResult.h"
#include "RooSimultaneous.h"
#include "RooMCStudy.h"

#include "ShapeMaker.hpp"

int main(int argc, char * argv[]) {

    // ===============
    // Read input args
    // ===============
    // Check for parameters
    if (argc != 4 && argc != 8 && argc != 9) {
        std::cout << "Usage: ./FitTwoBody <2011:2012:2015:2016> <Sum: Y/N> "
            "<Binned: Y/N> (Custom BDT cuts: <Kpi-cut> <piK-cut> <KK-cut> "
            "<pipi-cut> <Save-plots: Y/N>)" << std::endl;
        return -1;
    }

    // Year (colon separated string)
    std::string input_year = argv[1];

    // Sum over charge (Y/N)
    std::string sum = argv[2];

    // Choice of binned/unbinned
    std::string binned = argv[3];

    // Custom BDT cut if using
    bool custom_cuts = false;
    double cut_Kpi = 0.5;
    double cut_piK = 0.5;
    double cut_KK = 0.5;
    double cut_pipi = 0.5;
    std::string save_plots = "Y";
    if (argc >= 8) {
        custom_cuts = true;
        save_plots = "N";
        cut_Kpi = atof(argv[4]);
        cut_piK = atof(argv[5]);
        cut_KK = atof(argv[6]);
        cut_pipi = atof(argv[7]);
        if (argc == 9) save_plots = argv[8];
    }

    // Get name of results output
    std::string sum_string = ((sum == "Y") ? "combined" : "split");
    std::string bin_string = ((binned == "Y") ? "binned" : "unbinned");
    std::string result_filename;
    std::stringstream bdt_stream;

    // Filename of RooFitResult
    if (custom_cuts) {
        bdt_stream << "_" << std::setprecision(2) << cut_Kpi << "_" << cut_piK << "_"
            << cut_KK << "_" << cut_pipi;
        result_filename = "../Results/BDT_studies/twoBody_" + input_year + "_" +
            sum_string + "_" + bin_string + bdt_stream.str() + ".root";
    } else {
        result_filename = "../Results/twoBody_" + input_year + "_" + sum_string + 
            "_" + bin_string + ".root";
    }

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);

    // Make ShapeMaker
    ShapeMaker * sm = new ShapeMaker(sum, Bd_M);
    
    // Get toy PDF
    RooSimultaneous * toyPdf = sm->makeGenerationPdf(result_filename);
    toyPdf->Print("v");

    // Generate datasets with RooMCStudy
    int nToys = 1;
    int expectedEvents = toyPdf->expectedEvents(*sm->getCategory());
    RooMCStudy * mcstudy_gen = new RooMCStudy(*toyPdf, 
            RooArgList(*Bd_M, *sm->getCategory()), RooFit::Extended(true));
    mcstudy_gen->generate(nToys, expectedEvents, true, 0);
    RooDataSet * data = (RooDataSet*)mcstudy_gen->genData(0);
    data->Print("v");
    //RooDataSet * toyData = toyPdf->generate(*Bd_M, expectedEvents, 
            //RooFit::Index(*sm->getCategory()));
    //toyData->Print("v");
    //RooDataHist * toyHist = toyData->binnedClone();

    // Get fitting PDF (non-blind)
    RooSimultaneous * fitPdf = sm->makeFitPdf(false);

    // Perform fit to binned or unbinned dataset
    for (int i = 0; i < nToys; i++) {
        RooFitResult * result;
        if (binned == "Y") {
            std::cout << "Performing binned fit" << std::endl;
            RooDataHist * hist = (RooDataHist*)((RooDataSet*)mcstudy_gen->genData(i))->binnedClone();
            hist->Print("v");
            result = fitPdf->fitTo(*hist, RooFit::Save(), RooFit::NumCPU(8, 2),
                    RooFit::Optimize(false), RooFit::Offset(true), 
                    RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
        } else {
            std::cout << "Performing unbinned fit" << std::endl;
            RooDataSet * data = (RooDataSet*)mcstudy_gen->genData(i);
            result = fitPdf->fitTo(*data, RooFit::Save(), RooFit::NumCPU(8, 2),
                    RooFit::Optimize(false), RooFit::Offset(true), 
                    RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
        }
        result->Print("v");
    }

    //// Save results

    return 0;
}
