#ifndef DATA_PDF_MAKER_HPP
#define DATA_PDF_MAKER_HPP

#include "RooDataHist.h"
#include "RooAbsData.h"

#include "ShapeMakerBase.hpp"
class RooDataSet;

// ==========================================
// Class for making a PDF for fitting to data
// ==========================================
class DataPdfMaker : public ShapeMakerBase {

public:
    DataPdfMaker(RooRealVar * x, RooCategory * cat, bool blind = true, 
            bool split_obs = false);
    DataPdfMaker(std::string name, RooRealVar * x, RooCategory * cat, 
            bool blind = true, bool split_obs = true);

    void SetZeroYield(std::string mode = "piK");
    void SetFixedPar(std::string name, double val);

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
    void SaveHistograms(std::string filename, RooAbsData * data, bool binned);

    // Get bias corrected variables
    std::map<std::string, RooFormulaVar*> GetBiasCorrections();

    // Apply sWeights to a file
    void ApplyWeights(std::string mode, RooDataSet * data, 
            std::vector<std::string> filenames, RooFitResult * r);

protected:

    // virtual void SetFloatingParameters();
    // virtual void SetConstantParameters();
    // void SetDependentParameters();
    bool m_blind;
    bool m_split_obs;
    bool m_splitRuns;
    virtual void MakeComponents();
    bool IsSplit();
    std::vector<std::string> Runs();
    std::vector<std::string> ObsRuns();

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
    bool SplitRuns();
    std::map<std::string, bool> m_zeroYields;
    bool m_asyms_made;
    bool m_ratios_made;

    // Lists of asymmetries and ratios
    std::vector<std::string> m_asyms_B0;
    std::vector<std::string> m_asyms_Bs;

    // Constants
    const double m_Bmass = 5279.63;
    const double m_Brange = 50.0;

    // Yields to set to zero or fix
    std::vector<std::string> m_zero_pars;
    std::map<std::string, double> m_fixed_pars;

};

#endif // DATA_PDF_MAKER_HPP
