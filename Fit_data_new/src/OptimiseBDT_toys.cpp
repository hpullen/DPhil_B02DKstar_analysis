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
    if (argc != 6) {
        std::cout << "Usage: ./StudyBDTtoys <Kpi-cut> <KK-cut> <pipi-cut> "
            "Kpipipi-cut> <pipipipi-cut>" << std::endl;
        return -1;
    }
    std::map<std::string, double> cuts;
    cuts["Kpi"] = atof(argv[1]);
    cuts["KK"] = atof(argv[2]);
    cuts["pipi"] = atof(argv[1]);
    cuts["Kpipipi"] = atof(argv[1]);
    cuts["pipipipi"] = atof(argv[1]);

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
        cat->defineType((mode + "_plus").c_str());
        cat->defineType((mode + "_minus").c_str());
    }
    ToyPdfMaker * toy_pdf = new ToyPdfMaker("toy", Bd_M, cat, fit_result_file);

    // Make fit PDF
    DataPdfMaker * fit_pdf = new DataPdfMaker("fit_shape", Bd_M, cat, false);

    // Make fitter
    ToyFitter * fitter = new ToyFitter(toy_pdf);
    fitter->AddFitPdf(fit_pdf);

    // Perform study
    std::string outfile = "Results/BDT/Toys/toy_fit" + ss.str() + ".root";
    fitter->PerformFits(outfile, 1);

    delete fitter;
    delete toy_pdf;
    delete fit_pdf;

    return 0;
}
