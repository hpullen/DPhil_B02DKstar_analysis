#ifndef TOY_BDT_PDF_MAKER_HPP
#define TOY_BDT_PDF_MAKER_HPP

#include <string>

#include "RooRealVar.h"
#include "RooCategory.h"

#include "ToyPdfMaker.hpp"

// =====================================================================
// Class for making a toy PDF with each yield rescaled by BDT efficiency
// =====================================================================
class ToyBDTPdfMaker : public ToyPdfMaker {

public:
    ToyBDTPdfMaker(RooRealVar * x, RooCategory * cat, double cut_Kpi, double cut_KK, 
            double cut_pipi, double cut_Kpipipi, double cut_pipipipi);
    ~ToyBDTPdfMaker();

private:
    void RescaleYields();
    void MakeShape();
    std::map<std::string, double> MakeCutMap(double cut_Kpi, double cut_KK, 
            double cut_pipi, double cut_Kpipipi, double cut_pipipipi);
    std::map<std::string, std::map<std::string, double>> GetEfficiencies();
    double GetEfficiency(std::string filename, double cut);

    std::map<std::string, double> m_cuts;
};

#endif // TOY_BDT_PDF_MAKER_HPP
