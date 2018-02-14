#ifndef SHAPE_MAKER_BASE_HPP
#define SHAPE_MAKER_BASE_HPP

#include <string>

#include "RooSimultaneous.h"
#include "RooRealVar.h"

#include "ParameterManager.hpp"
#include "ShapeManager.hpp"

// ==========================================================
// Abstract base class for making a fit or toy generation PDF
// ==========================================================
class ShapeMakerBase {

public:
    ShapeMakerBase(std::string name, RooRealVar * x);
    virtual ~ShapeMakerBase();

    // Get shape
    RooSimultaneous * GetShape();

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
    virtual void MakeSimultaneousShape() = 0;

protected:
    std::string m_name;
    RooRealVar * m_x;
    RooSimultaneous * m_pdf;
    ParameterManager * m_pars;
    ShapeManager * m_shapes;
};

#endif // SHAPE_MAKER_BASE_HPP
