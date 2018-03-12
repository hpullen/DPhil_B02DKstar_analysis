#ifndef DATA_PDF_MAKER_HPP
#define DATA_PDF_MAKER_HPP

#include "ShapeMakerBase.hpp"

// ==========================================
// Class for making a PDF for fitting to data
// ==========================================
class DataPdfMaker : public ShapeMakerBase {

public:
    DataPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true);
    DataPdfMaker(std::string name, RooRealVar * x, RooCategory * cat, bool blind = true);

    void SetZeroYield(std::string mode = "piK", bool set_zero = true);

    virtual ~DataPdfMaker() {};

    // Override of histogram saving allowing for blindness
    void SaveHistograms(std::string filename);
    void SaveHistograms(std::string filename, RooDataHist * data);

protected:
    virtual void SetFloatingParameters();

private:
    // Parameter setup
    virtual void SetConstantParameters();
    void SetZeroParameters();
    void SetDependentParameters();
    void SetZeroYields();

    // Shape setup
    void MakeComponentShapes();
    void MakeModeShapes();

    // Properties
    bool m_blind;
    bool IsSplit();
    std::map<std::string, bool> m_zeroYields;

};

#endif // DATA_PDF_MAKER_HPP
