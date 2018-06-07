#ifndef CROSSCHECK_PDF_MAKER_HPP
#define CROSSCHECK_PDF_MAKER_HPP

#include "DataPdfMaker.hpp"

// ===================================================================
// Class for running a fit with yields floating instead of observables
// ===================================================================
class CrossCheckPdfMaker : public DataPdfMaker {

public:
    CrossCheckPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true);
    ~CrossCheckPdfMaker() {};

private:
    // Override function for making shape
    void MakeComponents();

};

#endif // CROSSCHECK_PDF_MAKER_HPP


