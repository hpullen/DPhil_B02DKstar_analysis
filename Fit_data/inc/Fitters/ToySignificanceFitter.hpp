#ifndef TOY_SIGNIFICANCE_FITTER_HPP
#define TOY_SIGNIFICANCE_FITTER_HPP

#include "ToyFitter.hpp"

// ===========================================================================
// Class for generating a toy and fitting it with a normal and null hypothesis
// ===========================================================================
class ToySignificanceFitter : private ToyFitter {

public: 
    ToySignificanceFitter();
    ToySignificanceFitter(std::string mode);
    ToySignificanceFitter(std::string mode, double cut);
    ~ToySignificanceFitter();
    
    void PerformFits(std::string filename, int n_repeats = 1);

private:
    RooCategory * m_cat;
    RooRealVar * m_x;
    std::string m_name;

    DataPdfMaker * MakeToyPdf(std::string mode);
    DataPdfMaker * MakeToyPdf(std::string mode, double cut);
    DataPdfMaker * MakeSignalPdf();
    DataPdfMaker * MakeNullPdf(std::string mode);

};

#endif // TOY_SIGNIFICANCE_FITTER_HPP
