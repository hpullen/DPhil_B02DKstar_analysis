#include "ToyFitter.hpp"
#include "ToyBDTPdfMaker.hpp"


// ================================
// BDT optimisation using just toys
// ================================
int main(int argc, char * argv[]) {

    // Get cuts from command line
    if (argc != 7) {
        std::cout << "Usage: ./StudyBDTtoys <ID> <Kpi-cut> <KK-cut> <pipi-cut> "
            "Kpipipi-cut> <pipipipi-cut>" << std::endl;
        return -1;
    }
    std::map<std::string, double> cuts;
    int ID = atoi(argv[1]);
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
        cat->defineType("Kpi_" + sign);
        cat->defineType("piK_" + sign);
        cat->defineType("KK_" + sign);
        cat->defineType("pipi_" + sign);
        cat->defineType("Kpipipi_" + sign);
        cat->defineType("piKpipi_" + sign);
        cat->defineType("pipipipi_" + sign);
    }

    // Generate toy
    ToyBDTPdfMaker * toy_pdf = new ToyBDTPdfMaker(Bd_M, cat, cuts["Kpi"], 
            cuts["KK"], cuts["pipi"], cuts["Kpipipi"], cuts["pipipipi"]);
    ToyFitter * fitter = new ToyFitter(toy_pdf);

    // Make name for output file
    std::string outfile_name = "toyFit_";
    for (auto mode : cuts) {
        outfile_name += mode.first;
        outfile_name += "_";
        outfile_name += std::to_string(mode.second);
    }
    outfile_name += std::to_string(ID);

    // Fit with normal PDF
    DataPdfMaker * fit_pdf = new DataPdfMaker("fit_shape", Bd_M, cat, false);
    fitter->AddFitPdf(fit_pdf);
    fitter->PerformFits("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
            + outfile_name + ".root", 20);

    delete fitter;
    delete toy_pdf;
    delete fit_pdf;;
    return 0;


}
