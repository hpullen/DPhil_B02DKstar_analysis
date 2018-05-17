#ifndef SYSTEMATIC_PDF_MAKER_HPP
#define SYSTEMATIC_PDF_MAKER_HPP

#include "DataPdfMaker.hpp"

// Options
enum SysOption {
    fs_fd
};

// =======================================================
// Class for making a PDF with constant parameters shifted
// =======================================================
class SystematicPdfMaker : public DataPdfMaker {

public:
    SystematicPdfMaker(SysOption opt, RooRealVar * x, RooCategory * cat, 
            bool blind = true);
    ~SystematicPdfMaker();

private: 
    // Override function for making shape
    void MakeShape();
    SysOption m_opt;
};

#endif // SYSTEMATIC_PDF_MAKER_HPP
