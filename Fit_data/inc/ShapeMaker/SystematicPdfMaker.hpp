#ifndef SYSTEMATIC_PDF_MAKER_HPP
#define SYSTEMATIC_PDF_MAKER_HPP

#include "DataPdfMaker.hpp"
#include "SysOption.hpp"

// =======================================================
// Class for making a PDF with constant parameters shifted
// =======================================================
class SystematicPdfMaker : public DataPdfMaker {

public:
    SystematicPdfMaker(SysOption opt, RooRealVar * x, RooCategory * cat, 
            bool blind, bool split_obs);
    ~SystematicPdfMaker();

private: 
    // Override function for making shape
    void MakeShape();
    SysOption m_opt;
};

#endif // SYSTEMATIC_PDF_MAKER_HPP
