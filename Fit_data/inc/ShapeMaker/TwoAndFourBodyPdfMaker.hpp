#ifndef TWO_BODY_PDF_MAKER_HPP
#define TWO_BODY_PDF_MAKER_HPP

#include "TwoAndFourBodyBase.hpp"

namespace TwoAndFourBody {
    enum Hypothesis {
        Signal,
        NullTwoBody,
        NullFourBody
    };
}
        

// ==================================================
// Class for making a fit PDF for two and four bodies
// ==================================================
class TwoAndFourBodyPdfMaker : public TwoAndFourBodyBase  {

public:
    TwoAndFourBodyPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true);
    TwoAndFourBodyPdfMaker(std::string name, RooRealVar * x, RooCategory * cat,
        bool blind = true);
    TwoAndFourBodyPdfMaker(RooRealVar * x, RooCategory * cat, 
            TwoAndFourBody::Hypothesis hyp, bool blind = true);
    TwoAndFourBodyPdfMaker(std::string name, RooRealVar * x, RooCategory * cat,
        TwoAndFourBody::Hypothesis hyp, bool blind = true);
    ~TwoAndFourBodyPdfMaker(); 

    // Override of histogram saving allowing for blindness
    void SaveHistograms(std::string filename);
    void SaveHistograms(std::string filename, RooDataHist * data);

private:
    void SetFloatingParameters();

    bool m_blind;
    TwoAndFourBody::Hypothesis m_hyp;
};

#endif // TWO_BODY_PDF_MAKER_HPP
