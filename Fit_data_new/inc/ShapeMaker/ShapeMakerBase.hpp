#ifndef SHAPE_MAKER_BASE_HPP
#define SHAPE_MAKER_BASE_HPP

#include <string>

class RooSimultaneousPdf;
class ParameterManager;

// ==========================================================
// Abstract base class for making a fit or toy generation PDF
// ==========================================================
class ShapeMakerBase {

public:
    ShapeMakerBase(std::string name);
    virtual ~ShapeMakerBase();

    // Get shape
    RooSimultaneousPdf * GetShape();

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
    RooSimultaneousPdf * m_shape;
    ParameterManager * m_params;
};

#endif // SHAPE_MAKER_BASE_HPP
