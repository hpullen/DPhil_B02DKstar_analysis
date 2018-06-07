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

    // Get integrals
    double GetSignalIntegral(std::string mode);
    double GetBackgroundIntegral(std::string mode);
    double GetCombinatorialIntegral(std::string mode);

    // Print observables
    void PrintAsymmetries();
    void PrintRatios();
    void PrintYields(RooFitResult * r);

    // Override of histogram saving allowing for blindness
    void SaveHistograms(std::string filename);
    void SaveHistograms(std::string filename, RooDataHist * data);

protected:

    // virtual void SetFloatingParameters();
    // virtual void SetConstantParameters();
    // void SetDependentParameters();
    bool m_blind;
    virtual void MakeComponents();

private:
    // Parameter setup
    // void SetZeroParameters();
    // void SetZeroYields();
    // Parameter setup
    void MakeModeShapes();
    void MakeSharedParameters();
    void MakeSignalShape();
    void MakeLowMassShape();
    void MakeRhoShape();
    void MakeDKpipiShape();
    void MakeCombiShape();

    // Properties
    bool IsSplit();
    std::vector<std::string> Runs();
    bool SplitRuns();
    bool m_splitRuns;
    std::map<std::string, bool> m_zeroYields;
    bool m_asyms_made;
    bool m_ratios_made;

    // Lists of asymmetries and ratios
    std::vector<std::string> m_asyms_B0;
    std::vector<std::string> m_asyms_Bs;

    // Constants
    const double m_Bmass = 5279.63;
    const double m_Brange = 50.0;

};

#endif // DATA_PDF_MAKER_HPP
