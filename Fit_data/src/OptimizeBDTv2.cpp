#include "ToyFitter.hpp"
#include "ToyBDTPdfMaker.hpp"


// ================================
// BDT optimisation using just toys
// ================================
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

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);

    // Make category
    RooCategory * cat = new RooCategory("modes", "");
    for (TString sign : {"plus", "minus"}) {
        for (TString run : {"_run1", "_run2"}) {
            cat->defineType("Kpi" + run + "_" + sign);
            cat->defineType("piK" + run + "_" + sign);
            cat->defineType("KK" + run + "_" + sign);
            cat->defineType("pipi" + run + "_" + sign);
            cat->defineType("Kpipipi" + run + "_" + sign);
            cat->defineType("piKpipi" + run + "_" + sign);
            if (run != "_run1") {
                cat->defineType("pipipipi" + run + "_" + sign);
            }
        }
    }

    // Generate toy
    ToyBDTPdfMaker * toy_pdf = new ToyBDTPdfMaker(Bd_M, cat, cuts["Kpi"], 
            cuts["KK"], cuts["pipi"], cuts["Kpipipi"], cuts["pipipipi"]);
    ToyFitter * fitter = new ToyFitter(toy_pdf, false);

    // Fit with normal PDF
    DataPdfMaker * fit_pdf = new DataPdfMaker("fit_shape", Bd_M, cat, false);
    fitter->AddFitPdf(fit_pdf);
    fitter->PerformFits(outfile, 50);

    delete fitter;
    delete toy_pdf;
    delete fit_pdf;;
    return 0;


}
