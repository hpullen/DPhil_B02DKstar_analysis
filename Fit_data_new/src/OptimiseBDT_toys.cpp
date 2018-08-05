#include <iostream>
#include <sstream>

#include "RooRealVar.h"
#include "RooCategory.h"

#include "ToyFitter.hpp"
#include "ToyPdfMaker.hpp"
#include "DataPdfMaker.hpp"

// ================================================================
// Script to run many toys for one point in BDT cut parameter space
// ================================================================
int main(int argc, char * argv[]) {

    // Get cuts from command line
    if (argc != 7) {
        std::cout << "Usage: ./StudyBDTtoys <outfile> <Kpi-cut> <KK-cut> <pipi-cut> "
            "Kpipipi-cut> <pipipipi-cut>" << std::endl;
        return -1;
    }
    std::map<std::string, double> cuts;
    std::string outfile = argv[1];
    cuts["Kpi"] = atof(argv[2]);
    cuts["KK"] = atof(argv[3]);
    cuts["pipi"] = atof(argv[4]);
    cuts["Kpipipi"] = atof(argv[5]);
    cuts["pipipipi"] = atof(argv[6]);

    // Get name of RooFitResult file
    std::stringstream ss;
    for (auto mode : cuts) {
        ss << "_" << mode.first << "_" << mode.second;
    }
    std::string fit_result_file = "Results/BDT/Fit_results/data_fit" + ss.str() 
        + ".root";

    // Make toy fit shape
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    RooCategory * cat = new RooCategory("cat", "");
    for (std::string mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi",
            "pipipipi"}) {
        for (std::string run : {"_run1", "_run2"}) {
            if (mode == "pipipipi" && run == "_run1") continue;
            cat->defineType((mode + run + "_plus").c_str());
            cat->defineType((mode + run + "_minus").c_str());
        }
    }
    ToyPdfMaker * toy_pdf = new ToyPdfMaker("toy", Bd_M, cat, fit_result_file);

    // Make fit PDF
    DataPdfMaker * fit_pdf = new DataPdfMaker("fit_shape", Bd_M, cat, false);

    // Make fitter
    ToyFitter * fitter = new ToyFitter(toy_pdf, false);
    fitter->AddFitPdf(fit_pdf);

    // Perform study
    fitter->PerformFits(outfile, 10);

    delete fitter;
    delete toy_pdf;
    delete fit_pdf;

    return 0;
}
