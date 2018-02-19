#ifndef SIMPLE_PDF_MAKER_HPP
#define SIMPLE_PDF_MAKER_HPP

#include "SimpleShapeMakerBase.hpp"

// Enum for fixed vs. free piK yield
enum Hypothesis {
    Signal = 0,
    Null = 1
};

// ===============================================
// Class for making a simple PDF to generate toys 
// ===============================================
class SimplePdfMaker : public SimpleShapeMakerBase {

public:
    SimplePdfMaker(RooRealVar * x, RooCategory * cat);
    SimplePdfMaker(std::string name, RooRealVar * x, RooCategory * cat);
    SimplePdfMaker(RooRealVar * x, RooCategory * cat, Hypothesis hyp);
    SimplePdfMaker(std::string name, RooRealVar * x, RooCategory * cat, 
            Hypothesis hyp);
    ~SimplePdfMaker();

private:
    void SetFloatingParameters();
    Hypothesis m_hyp;

};

#endif // SIMPLE_PDF_MAKER_HPP
