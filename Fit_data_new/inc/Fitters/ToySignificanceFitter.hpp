#ifndef TOY_SIGNIFICANCE_FITTER_HPP
#define TOY_SIGNIFICANCE_FITTER_HPP

#include "ToyFitter.hpp"

namespace Fitter {
    enum ToyType {
        Simple, 
        TwoAndFourBody
    };
}

// ===========================================================================
// Class for generating a toy and fitting it with a normal and null hypothesis
// ===========================================================================
class ToySignificanceFitter : private ToyFitter {

public: 
    ToySignificanceFitter(Fitter::ToyType toy_type = Fitter::ToyType::Simple);
    ~ToySignificanceFitter();
    
    void PerformFits(std::string filename, int n_repeats = 1);

private:
    RooCategory * m_cat;
    RooRealVar * m_x;

    ShapeMakerBase * MakeToyPdf(Fitter::ToyType toy_type);
    ShapeMakerBase * MakeSignalPdf(Fitter::ToyType toy_type);
    ShapeMakerBase * MakeNullPdf(Fitter::ToyType toy_type);

};

#endif // TOY_SIGNIFICANCE_FITTER_HPP
