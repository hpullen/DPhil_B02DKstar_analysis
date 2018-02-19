#ifndef TOY_SIGNIFICANCE_FITTER_HPP
#define TOY_SIGNIFICANCE_FITTER_HPP

#include "ToyFitter.hpp"

// ===========================================================================
// Class for generating a toy and fitting it with a normal and null hypothesis
// ===========================================================================
class ToySignificanceFitter : private ToyFitter {

public: 
    ToySignificanceFitter();
    ~ToySignificanceFitter();
    
    void PerformFits(std::string filename, int n_repeats = 1);

private:
    RooCategory * m_cat;
    RooRealVar * m_x;

    ShapeMakerBase * MakeToyPdf();
    ShapeMakerBase * MakeSignalPdf();
    ShapeMakerBase * MakeNullPdf();

};

#endif // TOY_SIGNIFICANCE_FITTER_HPP
