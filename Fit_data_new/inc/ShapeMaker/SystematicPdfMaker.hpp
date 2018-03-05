#ifndef SYSTEMATIC_PDF_MAKER_HPP
#define SYSTEMATIC_PDF_MAKER_HPP

#include "TwoAndFourBodyPdfMaker.hpp"

// =======================================================
// Class for making a PDF with constant parameters shifted
// =======================================================
class SystematicPdfMaker : public TwoAndFourBodyPdfMaker {

public:
    SystematicPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true);
    ~SystematicPdfMaker();

private: 
    // Override function for setting constant parameters
    void SetConstantParameters();
};

#endif // SYSTEMATIC_PDF_MAKER_HPP
