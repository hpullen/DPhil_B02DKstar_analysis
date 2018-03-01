#ifndef SHAPE_MAKER_BASE_HPP
#define SHAPE_MAKER_BASE_HPP

#include <string>
#include <vector>

#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "RooRealVar.h"

#include "ParameterManager.hpp"
#include "ShapeManager.hpp"

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

    // Set the maximum yields using a RooDataHist 
    void SetMaxYields(RooDataHist * data);

    // Remake shape
    RooSimultaneous * RemakeShape();

    // Save histograms to a file
    void SaveHistograms(std::string filename, bool blind = false);
    void SaveHistograms(std::string filename, RooDataHist * data, 
            bool blind = false);

protected:
    
    // Main functions for creating the PDF
    virtual void MakeShape();

    // Subfunctions for the PDF creation process
    // Parameter setup
    virtual void SetConstantParameters() = 0;
    virtual void SetFloatingParameters() = 0;
    virtual void SetDependentParameters() = 0;

    // Shape setup
    virtual void MakeComponentShapes() = 0;
    virtual void MakeModeShapes() = 0;
    void MakeSimultaneousShape();

protected:
    std::string m_name;
    RooRealVar * m_x;
    std::vector<std::string> m_modes;
    RooSimultaneous * m_pdf;
    ParameterManager * m_pars;
    ShapeManager * m_shapes;
    double GetMaxYield(std::string mode);

private:
    RooCategory * m_cat;
    bool m_shapeMade;
    std::vector<std::string> m_parList;
    std::map<std::string, double> m_maxYields;
    const double m_defaultMaxYield = 8000;

    std::vector<std::string> MakeModeList(RooCategory * const cat); 
    void SaveFitShapes(TFile * file, bool blind);
    void SaveSingleFitShape(std::string mode, TFile * file, bool blind);
    bool ShouldBeBlind(std::string mode);
};

#endif // SHAPE_MAKER_BASE_HPP
