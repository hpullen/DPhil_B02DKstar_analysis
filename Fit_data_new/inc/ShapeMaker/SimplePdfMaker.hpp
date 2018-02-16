#ifndef SIMPLE_PDF_MAKER_HPP
#define SIMPLE_PDF_MAKER_HPP

#include "SimpleShapeMakerBase.hpp"

// ===============================================
// Class for making a simple PDF to generate toys 
// ===============================================
class SimplePdfMaker : public SimpleShapeMakerBase {

public:
    SimplePdfMaker(RooRealVar * x, RooCategory * cat);
    SimplePdfMaker(std::string name, RooRealVar * x, RooCategory * cat);
    ~SimplePdfMaker();

private:
    void SetFloatingParameters();

};


#endif // SIMPLE_PDF_MAKER_HPP
