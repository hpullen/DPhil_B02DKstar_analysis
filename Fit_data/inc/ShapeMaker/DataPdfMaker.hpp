#ifndef DATA_PDF_MAKER_HPP
#define DATA_PDF_MAKER_HPP

#include "RooDataHist.h"
#include "RooAbsData.h"

#include "ShapeMakerBase.hpp"

// ==========================================
// Class for making a PDF for fitting to data
// ==========================================
class DataPdfMaker : public ShapeMakerBase {

public:
    DataPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true);
    DataPdfMaker(std::string name, RooRealVar * x, RooCategory * cat, bool blind = true);

    void SetZeroYield(std::string mode = "piK");

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
    void SaveHistograms(std::string filename, RooAbsData * data);

    // Get R_ds calculated value
    RooFormulaVar * GetR_ds(std::string mode, std::string run);

    // Set R± to be separate for each run
    void SeparateRruns();

protected:

    // virtual void SetFloatingParameters();
    // virtual void SetConstantParameters();
    // void SetDependentParameters();
    bool m_blind;
    virtual void MakeComponents();
    bool IsSplit();

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

    // Split R± for run 1 and run 2
    bool m_sep_R;

};

#endif // DATA_PDF_MAKER_HPP
