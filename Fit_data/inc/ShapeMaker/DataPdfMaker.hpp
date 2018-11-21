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
    DataPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true, 
            bool split_GLW = true);
    DataPdfMaker(std::string name, RooRealVar * x, RooCategory * cat, 
            bool blind = true, bool split_GLW = true);

    void SetZeroYield(std::string mode = "piK");

    virtual ~DataPdfMaker() {};

    // Get integrals
    std::pair<double, double> GetSignalIntegral(std::string mode);
    std::pair<double, double> GetBackgroundIntegral(std::string mode);
    // double GetCombinatorialIntegral(std::string mode);

    // Print observables
    void PrintAsymmetries();
    void PrintRatios();
    void PrintYields(RooFitResult * r);

    // Override of histogram saving allowing for blindness
    void SaveHistograms(std::string filename);
    void SaveHistograms(std::string filename, RooAbsData * data);

    // Get R_ds calculated value
    RooFormulaVar * GetR_ds(std::string mode, std::string run);

    // Get bias corrected variables
    std::map<std::string, RooFormulaVar*> GetBiasCorrections();

    // Set R± to be separate for each run
    void SeparateRruns();

protected:

    // virtual void SetFloatingParameters();
    // virtual void SetConstantParameters();
    // void SetDependentParameters();
    bool m_blind;
    bool m_split_GLW;
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

    // Override of shape making
    void MakeShape();

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

    // Yields to set to zero
    std::vector<std::string> m_zero_pars;

};

#endif // DATA_PDF_MAKER_HPP
