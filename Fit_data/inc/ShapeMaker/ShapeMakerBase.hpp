#ifndef SHAPE_MAKER_BASE_HPP

#define SHAPE_MAKER_BASE_HPP

#include <string>
#include <vector>

#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "RooRealVar.h"
#include "RooAbsData.h"

#include "ParameterManager.hpp"
#include "ShapeManager.hpp"

class TH1F;
class RooArgSet;

// ==========================================================
// Abstract base class for making a fit or toy generation PDF
// ==========================================================
class ShapeMakerBase {

public:
    ShapeMakerBase(std::string name, RooRealVar * x, RooCategory * cat);
    virtual ~ShapeMakerBase();

    // Get shape and category
    RooSimultaneous * Shape();
    RooCategory * Category();
    RooRealVar * FitVariable();
    int ExpectedEvents();
    std::string Name();
    std::vector<std::string> Parameters();

    // Get current value and error of a parameter
    double GetParameterValue(std::string name);
    double GetParameterError(std::string name);
    void SetParameter(std::string name, double value);

    // Set the maximum yields using a RooDataHist 
    void SetMaxYields(RooAbsData * data);

    // Remake shape
    RooSimultaneous * RemakeShape();

    // Get integral of a shape
    std::pair<double, double> GetComponentIntegral(std::string shape, std::string yield,
            double min, double max);
    std::pair<double, double> GetFitIntegral(std::string mode, double min, double max);

    // Print parameters to a file
    void PrintToFile(std::string filename);

    // Save histograms to a file
    void SaveHistograms(std::string filename, bool blind = false);
    void SaveHistograms(std::string filename, RooAbsData * data, 
            bool blind = false);

    // Check/get constraints
    bool HasConstraints();
    RooArgSet * GetConstraintPdfs();

protected:
    
    // Main functions for creating the PDF
    virtual void MakeShape();

    // Subfunctions for 
    virtual void MakeComponents() = 0;
    virtual void MakeModeShapes() = 0;
    void MakeSimultaneousShape();

protected:
    std::string m_name;
    RooRealVar * m_x;
    std::vector<std::string> m_modes;
    bool m_shapeMade;
    RooSimultaneous * m_pdf;
    ParameterManager * m_pars;
    ShapeManager * m_shapes;
    double GetMaxYield(std::string mode);

private:
    RooCategory * m_cat;
    std::vector<std::string> m_parList;
    std::map<std::string, double> m_maxYields;
    const double m_defaultMaxYield = 8000;

    std::vector<std::string> MakeModeList(RooCategory * const cat); 
    void SaveFitShapes(TFile * file, bool blind);
    void SaveSingleFitShape(std::string mode, TFile * file, bool blind);
    bool ShouldBeBlind(std::string mode);

    double IntegrateInRange(TH1F * hist, double min, double max);
    std::pair<double, double> GetComponentIntegral(RooAbsPdf * pdf, RooRealVar * yield, 
            double min, double max);
};

#endif // SHAPE_MAKER_BASE_HPP
