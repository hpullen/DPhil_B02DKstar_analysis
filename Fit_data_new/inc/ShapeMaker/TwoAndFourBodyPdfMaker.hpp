#ifndef TWO_BODY_PDF_MAKER_HPP
#define TWO_BODY_PDF_MAKER_HPP

#include "TwoAndFourBodyBase.hpp"


// ===================================
// Class for making a two-body fit PDF
// ===================================
class TwoAndFourBodyPdfMaker : public TwoAndFourBodyBase  {

public:
    TwoAndFourBodyPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true);
    TwoAndFourBodyPdfMaker(std::string name, RooRealVar * x, RooCategory * cat,
        bool blind = true);
    ~TwoAndFourBodyPdfMaker(); 

    // Override of histogram saving allowing for blindness
    void SaveHistograms(std::string filename);
    void SaveHistograms(std::string filename, RooDataHist * data);

private:
    void SetFloatingParameters();

    bool m_blind;
};

#endif // TWO_BODY_PDF_MAKER_HPP
