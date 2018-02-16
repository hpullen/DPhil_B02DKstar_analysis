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

    // Save histograms to a file
    void SaveHistograms(std::string filename);

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

private:
    std::vector<std::string> MakeModeList(RooCategory * const cat); 
    RooCategory * m_cat;
    bool m_shapeMade;
};

#endif // SHAPE_MAKER_BASE_HPP
