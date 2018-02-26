#ifndef TWO_BODY_PDF_MAKER_HPP
#define TWO_BODY_PDF_MAKER_HPP

#include "TwoBodyBase.hpp"


// ===================================
// Class for making a two-body fit PDF
// ===================================
class TwoBodyPdfMaker : public TwoBodyBase {

public:
    TwoBodyPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true);
    TwoBodyPdfMaker(std::string name, RooRealVar * x, RooCategory * cat,
        bool blind = true);
    ~TwoBodyPdfMaker(); 

    // Override of histogram saving allowing for blindness
    void SaveHistograms(std::string filename);
    void SaveHistograms(std::string filename, RooDataHist * data);

private:
    void SetFloatingParameters();
    void SaveBlindHistograms(std::string filename);
    void SaveBlindHistograms(std::string filename, RooDataHist * data);

    bool m_blind;

};

#endif // TWO_BODY_PDF_MAKER_HPP
