#ifndef DATA_PDF_MAKER_HPP
#define DATA_PDF_MAKER_HPP

#include "ShapeMakerBase.hpp"

// Enum for hypothesis of fit
enum Hypothesis {
    Signal,
    NullTwoBody,
    NullFourBody
};


// ================================
// Base class for a two-body fitter
// ================================
class DataPdfMaker : public ShapeMakerBase {

public:
    DataPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true);
    DataPdfMaker(std::string name, RooRealVar * x, RooCategory * cat, bool blind = true);
    DataPdfMaker(RooRealVar * x, RooCategory * cat, Hypothesis hyp, bool blind = true);
    DataPdfMaker(std::string name, RooRealVar * x, RooCategory * cat, Hypothesis hyp, bool blind = true);

    virtual ~DataPdfMaker() {};

    // Override of histogram saving allowing for blindness
    void SaveHistograms(std::string filename);
    void SaveHistograms(std::string filename, RooDataHist * data);

private:
    // Parameter setup
    virtual void SetConstantParameters();
    void SetZeroParameters();
    void SetDependentParameters();
    virtual void SetFloatingParameters();

    // Shape setup
    void MakeComponentShapes();
    void MakeModeShapes();

    // Properties
    bool m_blind;
    Hypothesis m_hyp;
    bool IsSplit();

};

#endif // DATA_PDF_MAKER_HPP
